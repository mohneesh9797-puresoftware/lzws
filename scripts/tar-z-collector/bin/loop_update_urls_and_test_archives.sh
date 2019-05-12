#!/bin/sh
set -e

cd "$(dirname $0)"

UPDATE_URLS_TIMES=3

while true; do
  for i in $(seq $UPDATE_URLS_TIMES); do
    ./update_urls.sh
  done

  ./test_archives.sh
done
