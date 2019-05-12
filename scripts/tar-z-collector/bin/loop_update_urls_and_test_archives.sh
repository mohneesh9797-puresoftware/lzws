#!/bin/sh
set -e

cd "$(dirname $0)"

UPDATE_URLS_TIMES=10

while true; do
  for i in {1..$UPDATE_URLS_TIMES}; do
    ./update_urls.sh
  done

  ./test_archives.sh
done
