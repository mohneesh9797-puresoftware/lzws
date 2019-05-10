#!/bin/sh
set -e

cd "$(dirname $0)"

tmp="../tmp"
build="$tmp/toolchain-build"

mkdir -p "$build"
cd "$build"

# We need to tests builds for all possible toolchains and dictionaries.
toolchains="../../cmake/toolchains"

if [ $(uname -s) = "Darwin" ]; then
  toolchains="$toolchains/osx"
else
  toolchains="$toolchains/linux"
fi

find "$toolchains" -type f | while read -r toolchain; do
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
      -DLZWS_MAN=0 \
      -DCMAKE_BUILD_TYPE="RELEASE" \
      -DCMAKE_C_FLAGS_RELEASE="-O2 -march=native"
    make clean
    make -j2

    CTEST_OUTPUT_ON_FAILURE=1 make test
  done
done
