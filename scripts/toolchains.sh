#!/bin/bash
set -e

cd "$(dirname $0)"

tmp="../tmp"
build="$tmp/toolchain-build"

mkdir -p "$build"
cd "$build"

# We need to make test builds for all possible toolchains and dictionaries.
toolchains="../../cmake/toolchains"

some_test_passed=false

while read -r toolchain; do
  for dictionary in "linked-list" "sparse-array"; do
    echo "toolchain: $toolchain, dictionary: $dictionary"

    find . \( -name "CMake*" -o -name "*.cmake" \) -exec rm -rf {} +

    # Toolchain may not work on target platform.
    cmake "../.." \
      -DCMAKE_TOOLCHAIN_FILE="$toolchain" \
      -DLZWS_COMPRESSOR_DICTIONARY="$dictionary" \
      -DLZWS_SHARED=ON \
      -DLZWS_STATIC=ON \
      -DLZWS_CLI=OFF \
      -DLZWS_TESTS=ON \
      -DLZWS_EXAMPLES=ON \
      -DLZWS_MAN=OFF \
      -DCMAKE_BUILD_TYPE="RELEASE" \
      -DCMAKE_C_FLAGS_RELEASE="-O2 -march=native" \
      || continue
    make clean
    make -j2

    CTEST_OUTPUT_ON_FAILURE=1 make test

    some_test_passed=true
  done
done < <(find "$toolchains" -type f)

if [ "$some_test_passed" = false ]; then
  echo "At least one test should pass" > "/dev/stderr"
  exit 1
fi
