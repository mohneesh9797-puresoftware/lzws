#!/bin/sh
set -e

cd "$(dirname $0)"
base=$(pwd)

tmp="../../../tmp"
cd "$tmp"

# We need to create release builds for all possible dictionaries.
for dictionary in "linked-list" "sparse-array"; do
  build="tar-z-collector-$dictionary-build"
  mkdir -p "$build"
  cd "$build"

  find . -name "CMake*" -o -name "*.cmake" -exec rm -r {} +

  cmake "../.." \
    -DLZWS_COMPRESSOR_DICTIONARY="$dictionary" \
    -DLZWS_SHARED=0 \
    -DLZWS_STATIC=1 \
    -DLZWS_CLI=1 \
    -DLZWS_TESTS=0 \
    -DLZWS_MAN=0 \
    -DCMAKE_BUILD_TYPE=RELEASE \
    -DCMAKE_C_FLAGS_RELEASE="-O2 -march=native"
  make clean
  make -j2

  cd ".."
done

cd "$base"
torsocks "../test_archives/main.rb" "../data/archive_urls.xz" "../data/valid_archives.xz" "../data/invalid_archives.xz" "../data/volatile_archives.xz"
