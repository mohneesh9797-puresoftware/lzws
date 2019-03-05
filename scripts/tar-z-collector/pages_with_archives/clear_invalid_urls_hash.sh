#!/bin/sh
set -e

cd "$(dirname $0)"

truncate --size=0 "invalid_urls_hash.txt"
