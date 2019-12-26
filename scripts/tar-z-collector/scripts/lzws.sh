#!/bin/bash
set -e

DIR=$(dirname "${BASH_SOURCE[0]}")
cd "$DIR"

dictionary="$1"
shift

"../../../tmp/tar-z-collector-$dictionary-build/src/cli/lzws-static" $@
