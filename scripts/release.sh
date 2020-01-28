#!/bin/bash
set -e

DIR=$(dirname "${BASH_SOURCE[0]}")
cd "$DIR"

CPU_COUNT=$(grep -c "^processor" "/proc/cpuinfo")

cd "../build"

find . \( -name "CMake*" -o -name "*.cmake" \) -exec rm -rf {} +

for dictionary in "linked-list" "sparse-array"; do
  cmake ".." \
    -DLZWS_COMPRESSOR_DICTIONARY="$dictionary" \
    -DCMAKE_BUILD_TYPE="RELEASE"
  make clean
  make -j${CPU_COUNT} VERBOSE=1

  CTEST_OUTPUT_ON_FAILURE=1 make test

  make package
done
