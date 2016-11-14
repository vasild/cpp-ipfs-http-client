#!/bin/sh -ex

export IPFS_PATH="${TRAVIS_BUILD_DIR}/../ipfs-repository"

rm -fr "${IPFS_PATH}"
"${GO_IPFS_CMD}" init
"${GO_IPFS_CMD}" daemon > ipfs_daemon.out 2>&1 &
while ! grep -i 'Daemon is ready' ipfs_daemon.out ; do
  echo "$(date) Waiting for ipfs daemon to start..."
  sleep 1
done

# EOF
