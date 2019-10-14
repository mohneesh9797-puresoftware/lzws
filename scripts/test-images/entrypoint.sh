#!/bin/bash
set -e

cd "$(dirname $0)"

env-update
source /etc/profile

git clone "https://github.com/andrew-aladev/lzws.git" --single-branch --branch "master" --depth 1 "lzws"
cd "lzws"

./scripts/toolchains.sh
