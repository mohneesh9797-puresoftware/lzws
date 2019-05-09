#!/bin/sh
set -e

cd "$(dirname $0)"

tmp="../tmp"
build="$tmp/toolchain-build"

mkdir -p "$build"
cd "$build"

toolchains_dir="../../cmake/toolchains"

# We need to tests builds for all possible toolchains and dictionaries.
process_toolchain () {
  for dictionary in "linked-list" "sparse-array"; do
    find . \( -name "CMake*" -o -name "*.cmake" \) -exec rm -rf {} +

    cmake "../.." \
      -DCMAKE_TOOLCHAIN_FILE="$1" \
      -DLZWS_COMPRESSOR_DICTIONARY="$dictionary" \
      -DLZWS_SHARED=1 \
      -DLZWS_STATIC=1 \
      -DLZWS_CLI=0 \
      -DLZWS_TESTS=1 \
      -DLZWS_MAN=0
    make clean
    make -j2

    echo "toolchain: $1, dictionary: $dictionary"

    CTEST_OUTPUT_ON_FAILURE=1 make test
  done
}

export -f process_toolchain
find "$toolchains_dir" -type f -exec sh -c 'process_toolchain "$0"' {} \;
