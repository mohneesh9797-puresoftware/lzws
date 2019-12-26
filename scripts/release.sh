#!/bin/bash
set -e

DIR=$(dirname "${BASH_SOURCE[0]}")
cd "$DIR"

cd "../build"

find . \( -name "CMake*" -o -name "*.cmake" \) -exec rm -rf {} +

for dictionary in "linked-list" "sparse-array"; do
  cmake ".." \
    -DLZWS_COMPRESSOR_DICTIONARY="$dictionary" \
    -DCMAKE_BUILD_TYPE="RELEASE"
  make clean
  make -j2 VERBOSE=1

  CTEST_OUTPUT_ON_FAILURE=1 make test

  make package
done
