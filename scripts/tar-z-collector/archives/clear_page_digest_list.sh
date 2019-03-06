#!/bin/sh
set -e

cd "$(dirname $0)"

truncate --size=0 "page_digest_list.txt"
