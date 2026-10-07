
#!/usr/bin/env bash
set -euo pipefail

run_dir="$PWD/data"
printf '\nRun directory: %s\n\n' "$run_dir"

data_file="$run_dir/traced.bin"

strace -yy -s 40 \
    -e trace=openat,close,pread64,pwrite64,newfstatat,fstat,statx \
    -o "$run_dir/syscalls.txt" \
    ./build/sail "$data_file" 

grep -F "traced.bin" "$run_dir/syscalls.txt"

rm -f "$data_file"
