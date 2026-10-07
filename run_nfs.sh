#!/usr/bin/env bash
set -euo pipefail

cd -- "$(dirname -- "${BASH_SOURCE[0]}")"

mount_dir="/mnt/sail-nfs"

if [[ "$(findmnt -n -o FSTYPE -T "$mount_dir")" != "nfs4" ]]; then
    printf 'Error: %s is not on an NFSv4 mount.\n' "$mount_dir" >&2
    exit 1
fi

data_file="$mount_dir/sample.bin"

# Reset only s generated file through the NFS mount.
rm -f -- "$data_file"

printf 'Running Sail against: %s\n\n' "$data_file"
./build/sail "$data_file"

stat -c '%n: %s bytes' \
    /mnt/sail-nfs/sample.bin \
    /srv/sail-nfs/export/sample.bin

nfsstat -c -4
