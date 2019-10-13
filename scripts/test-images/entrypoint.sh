#!/bin/bash
set -e

cd "$(dirname $0)"

git clone "https://github.com/andrew-aladev/lzws.git" "lzws" --depth 1
cd "lzws"
"./scripts/toolchains.sh"
