#!/bin/sh
set -e

cd "$(dirname $0)"

torsocks "../update/main.rb" "../data/valid_page_url.list" "../data/invalid_page_url.list" "../data/archive_url.list"
