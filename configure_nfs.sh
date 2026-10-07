sudo mkdir -p /etc/exports.d

sudo tee /etc/exports.d/sail.exports > /dev/null <<'EOF'
/srv/sail-nfs/export 127.0.0.1(rw,sync,no_subtree_check,root_squash,fsid=0)
EOF

sudo mkdir -p /etc/nfs.conf.d

sudo tee /etc/nfs.conf.d/sail.conf > /dev/null <<'EOF'
[nfsd]
host = 127.0.0.1
vers3 = n
vers4 = y
tcp = y
udp = n
EOF


