#!/bin/sh
set -e

cd "$(dirname $0)"

while true; do
  ./test_archives.sh
done
