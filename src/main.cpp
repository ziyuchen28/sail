#include <algorithm>
#include <cerrno>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <string>
#include <system_error>
#include <vector>

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

[[noreturn]] void fail_system_call(const char *msg)
{
    const int error = errno;
    throw std::system_error(error, std::generic_category(), msg);
}

void pwrite_all(int fd, const char *data, std::size_t size, off_t offset)
{
    std::size_t done = 0;
    while (done < size) {
        const ssize_t n = ::pwrite(
            fd, data + done, size - done, offset + static_cast<off_t>(done));

        if (n == -1) {
            if (errno == EINTR) {
                continue;
            }
            fail_system_call("pwrite");
        }
        if (n == 0) {
            throw std::runtime_error("pwrite made no progress");
        }
        done += static_cast<std::size_t>(n);
    }
}

// Fill the buffer or stop at EOF. Return the number of bytes actually read.
std::size_t pread_up_to(int fd, char *data, std::size_t size, off_t offset)
{
    std::size_t done = 0;
    while (done < size) {
        const ssize_t n = ::pread(
            fd, data + done, size - done, offset + static_cast<off_t>(done));

        if (n == -1) {
            if (errno == EINTR) {
                continue;
            }
            fail_system_call("pread");
        }
        if (n == 0) {
            break;
        }
        done += static_cast<std::size_t>(n);
    }
    return done;
}

void close_file(int &fd)
{
    const int old_fd = fd;
    fd = -1; 
    if (::close(old_fd) == -1) {
        fail_system_call("close");
    }
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <new-file>\n";
        return 1;
    }

    // Purpose for below is to trigger read/write system(VFS) calls to be used by strace
    int fd = -1;
    try {
        constexpr std::size_t chunk_size = 4096;
        std::vector<char> expected(3 * chunk_size);
        std::fill_n(expected.begin(), chunk_size, 'A');
        std::fill_n(expected.begin() + chunk_size, chunk_size, 'B');
        std::fill_n(expected.begin() + 2 * chunk_size, chunk_size, 'C');

        const char *path = argv[1];
        fd = ::open(path, O_RDWR | O_CREAT | O_EXCL | O_CLOEXEC, 0644);
        if (fd == -1) {
            fail_system_call("open new file");
        }

        pwrite_all(fd, expected.data(), expected.size(), 0);
        std::cout << "Wrote " << expected.size()
                  << " bytes: 4096 A, 4096 B, 4096 C.\n";

        const std::string patch = "sail";
        pwrite_all(fd, patch.data(), patch.size(), 4094);
        // Patch the memory as well.
        std::copy(patch.begin(), patch.end(), expected.begin() + 4094);

        // Read the surrounding eight bytes and display what came back.
        char buffer[8] {};
        const std::size_t n = pread_up_to(fd, buffer, sizeof(buffer), 4092);
        std::cout << "Read " << n << " bytes: " << std::endl;
        std::cout << buffer << std::endl;
        std::cout << '\n';

        close_file(fd);
        fd = ::open(path, O_RDONLY | O_CLOEXEC);
        if (fd == -1) {
            fail_system_call("reopen");
        }
        std::cout << "Reopened read-only.\n";

        struct stat info {};
        if (::fstat(fd, &info) == -1) {
            fail_system_call("fstat");
        }
        if (info.st_size != static_cast<off_t>(expected.size())) {
            throw std::runtime_error("unexpected file size");
        }
        std::cout << "Metadata: inode="
                  << static_cast<std::uintmax_t>(info.st_ino)
                  << " size=" << info.st_size
                  << " links=" << info.st_nlink << '\n';

        close_file(fd);
        std::cout << "All checks passed.\n";
        return 0;
    } catch (const std::exception &error) {
        if (fd != -1) {
            ::close(fd);  
        }
        std::cerr << "sail: " << error.what() << '\n';
        return 1;
    }
}
