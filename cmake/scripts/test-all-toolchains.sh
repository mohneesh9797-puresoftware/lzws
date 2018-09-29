#!/bin/bash
set -e

cd "$(dirname $0)"

BUILD_DIR="../../build"

readarray -d '' toolchains < <(find "../toolchains" -print0)

for toolchain in "${toolchains[@]}"; do
  cd "$BUILD_DIR"
  rm -r "*"
  cmake ".." -DCMAKE_TOOLCHAIN_FILE="$toolchain" -DCMAKE_BUILD_TYPE=RELEASE
  make VERBOSE=1
  make test
done
