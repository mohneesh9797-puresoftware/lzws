#!/bin/bash
set -e

cd "$(dirname $0)"

tmp="../../../tmp"
dictionary="$1"
shift

"$tmp/tar-z-collector-$dictionary-build/src/cli/lzws-static" $@
