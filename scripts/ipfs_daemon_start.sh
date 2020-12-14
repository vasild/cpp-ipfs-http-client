#!/bin/sh -ex

"${GO_IPFS_CMD}" init
"${GO_IPFS_CMD}" daemon > ipfs_daemon.out 2>&1 &
while ! grep -i 'Daemon is ready' ipfs_daemon.out ; do
  echo "Waiting for ipfs daemon to start..."
  sleep 2
done

# EOF
