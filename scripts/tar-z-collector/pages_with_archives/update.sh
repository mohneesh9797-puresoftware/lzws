#!/bin/sh
set -e

cd "$(dirname $0)"

torsocks "./update.rb" "./url.list" "./bad_url.list"
