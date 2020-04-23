#!/bin/bash
set -e

DIR=$(dirname "${BASH_SOURCE[0]}")
cd "$DIR"

cd ".."

../mount_tmp.sh

./update_urls/main.rb \
  "data/valid_page_urls.xz" \
  "data/invalid_page_urls.xz" \
  "data/archive_urls.xz"
