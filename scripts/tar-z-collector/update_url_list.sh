#!/bin/sh
set -e

cd "$(dirname $0)"

# Proxychains is better than torsocks because it works correctly with timeouts.
proxychains "./update_url_list.rb" > "url.list"
