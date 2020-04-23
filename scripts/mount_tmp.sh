#!/bin/bash
set -e

DIR=$(dirname "${BASH_SOURCE[0]}")
cd "$DIR"

tmp_path=$(readlink -f "../tmp")
if mount | grep "$tmp_path" > /dev/null 2>&1; then
  echo "tmp is already mounted"
  exit 0
fi

# "sudo" may be required for ramfs.
if command -v sudo > /dev/null 2>&1; then
  sudo ./tmp_ramfs.sh || true
else
  ./tmp_ramfs.sh || true
fi
