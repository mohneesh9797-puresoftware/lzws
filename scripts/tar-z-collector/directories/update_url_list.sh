#!/bin/sh
set -e

cd "$(dirname $0)"

proxychains -f "../proxychains.conf" "./update_url_list.rb" < "./url.list" > "./url.list.tmp"
mv "./url.list.tmp" "./url.list"
