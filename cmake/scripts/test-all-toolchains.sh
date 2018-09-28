#!/bin/bash
set -e

cd "$(dirname $0)"

BUILD_DIR="../../build"

readarray -d '' toolchains < <(find "../toolchains" -print0)

for toolchain in "${toolchains[@]}"; do
  cmake "$BUILD_DIR" -DCMAKE_TOOLCHAIN_FILE="$toolchain" -DCMAKE_BUILD_TYPE=RELEASE
  make -C "$BUILD_DIR" VERBOSE=1
  make -C "$BUILD_DIR" test
done
