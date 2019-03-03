#!/bin/sh
set -e

cd "$(dirname $0)"

torsocks "./update.rb" "url.list" "invalid_url.list"
