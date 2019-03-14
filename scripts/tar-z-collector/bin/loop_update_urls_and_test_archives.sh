#!/bin/sh
set -e

cd "$(dirname $0)"

while true; do
  ./update_urls.sh
  ./test_archives.sh
done
