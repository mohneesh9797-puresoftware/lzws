#!/bin/sh
set -e

cd "$(dirname $0)"

truncate --size=0 "urls_hash.txt"
