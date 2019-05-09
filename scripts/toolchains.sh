#!/bin/sh
set -e

cd "$(dirname $0)"

tmp="../tmp"
build="$tmp/toolchain-build"

mkdir -p "$build"
cd "$build"

# We need to tests builds for all possible toolchains and dictionaries.
toolchains_dir="../../cmake/toolchains"

find "$toolchains_dir" -type f | while read -r toolchain; do
  for dictionary in "linked-list" "sparse-array"; do
    echo "toolchain: $toolchain, dictionary: $dictionary"

    find . \( -name "CMake*" -o -name "*.cmake" \) -exec rm -rf {} +

    cmake "../.." \
      -DCMAKE_TOOLCHAIN_FILE="$toolchain" \
      -DLZWS_COMPRESSOR_DICTIONARY="$dictionary" \
      -DLZWS_SHARED=1 \
      -DLZWS_STATIC=1 \
      -DLZWS_CLI=0 \
      -DLZWS_TESTS=1 \
      -DLZWS_MAN=0
    make clean
    make -j2

    CTEST_OUTPUT_ON_FAILURE=1 make test
  done
done
