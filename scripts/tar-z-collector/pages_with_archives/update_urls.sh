#!/bin/sh
set -e

cd "$(dirname $0)"

torsocks "./update_urls.rb" "url.list" "invalid_url.list"
