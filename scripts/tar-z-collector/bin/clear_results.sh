#!/bin/sh
set -e

cd "$(dirname $0)"

truncate --size=0 "../data/valid_archives.xz" "../data/invalid_archives.xz" "../data/volatile_archives.xz"
