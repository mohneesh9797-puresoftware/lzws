#!/bin/sh
set -e

cd "$(dirname $0)"

truncate --size=0 "../data/valid_page_urls.xz" "../data/invalid_page_urls.xz" "../data/archive_urls.xz"
