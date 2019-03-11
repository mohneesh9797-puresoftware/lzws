#!/bin/sh
set -e

cd "$(dirname $0)"

torsocks "../update/main.rb" "../data/valid_page_urls.xz" "../data/invalid_page_urls.xz" "../data/archive_urls.xz"
