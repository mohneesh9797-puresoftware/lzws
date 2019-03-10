#!/bin/sh
set -e

cd "$(dirname $0)"

truncate --size=0 "../data/valid_page_url.list" "../data/invalid_page_url.list" "../data/archive_url.list"
