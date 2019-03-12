#!/bin/sh
set -e

cd "$(dirname $0)"

tmp="../../../tmp"
dictionary="${1}"
shift

"${tmp}/${dictionary}-build/src/cli/lzws-static" $@
