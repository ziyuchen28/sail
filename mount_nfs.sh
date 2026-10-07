sudo mount -t nfs \
    -o vers=4.2,proto=tcp,hard \
    127.0.0.1:/ \
    /mnt/sail-nfs

findmnt -T /mnt/sail-nfs -o TARGET,SOURCE,FSTYPE,OPTIONS

findmnt -T /srv/sail-nfs/export -o TARGET,SOURCE,FSTYPE,OPTIONS
