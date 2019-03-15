#!/bin/sh
set -e

cd "$(dirname $0)"

while true; do
  ./update_urls.sh
done
