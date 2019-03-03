#!/bin/sh
set -e

cd "$(dirname $0)"

touch "bad_url.list"
torsocks "./update.rb" "url.list" "bad_url.list"
