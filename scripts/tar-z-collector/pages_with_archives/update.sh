#!/bin/sh
set -e

cd "$(dirname $0)"

torsocks "./update/main.rb" "url_hash.txt" "invalid_url_hash.txt"
