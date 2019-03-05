#!/bin/sh
set -e

cd "$(dirname $0)"

torsocks "./update/main.rb" "urls_hash.txt" "invalid_urls_hash.txt"
