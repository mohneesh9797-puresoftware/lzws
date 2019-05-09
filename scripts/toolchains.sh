#!/bin/sh
set -e

cd "$(dirname $0)"

tmp="../tmp"
build="$tmp/toolchain-build"

mkdir -p "$build"
cd "$build"

toolchains_dir="../../cmake/toolchains"

# We need to create builds for all possible toolchains and dictionaries.
find "$toolchains_dir" -type f -print0 | while read -d $'\0' toolchain; do
  for dictionary in "linked-list" "sparse-array"; do
    find . -name "CMake*" -o -name "*.cmake" -exec rm -r {} +

    cmake "../.." \
      -DCMAKE_TOOLCHAIN_FILE="$toolchain" \
      -DLZWS_COMPRESSOR_DICTIONARY="$dictionary" \
      -DLZWS_SHARED=0 \
      -DLZWS_STATIC=1 \
      -DLZWS_CLI=0 \
      -DLZWS_TESTS=1 \
      -DLZWS_MAN=0
    make clean
    make -j2

    echo "toolchain: $toolchain, dictionary: $dictionary"

    make test
  done
done
