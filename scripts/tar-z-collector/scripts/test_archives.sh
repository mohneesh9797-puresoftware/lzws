#!/bin/bash
set -e

DIR=$(dirname "${BASH_SOURCE[0]}")
cd "$DIR"

CPU_COUNT=$(grep -c "^processor" "/proc/cpuinfo" || sysctl -n "hw.ncpu")

cd ".."
BASE_DIR=$(pwd)

../mount_tmp.sh
cd "../../tmp"

# We need to create release builds for all possible dictionaries.
for dictionary in "linked-list" "sparse-array"; do
  build="tar-z-collector-${dictionary}-build"
  mkdir -p "$build"
  cd "$build"

  find . \( -name "CMake*" -o -name "*.cmake" \) -exec rm -rf {} +

  cmake "../.." \
    -DLZWS_COMPRESSOR_DICTIONARY="$dictionary" \
    -DLZWS_SHARED=OFF \
    -DLZWS_STATIC=ON \
    -DLZWS_CLI=ON \
    -DLZWS_TESTS=OFF \
    -DLZWS_EXAMPLES=OFF \
    -DLZWS_MAN=OFF \
    -DCMAKE_BUILD_TYPE="RELEASE" \
    -DCMAKE_C_FLAGS_RELEASE="-Ofast -march=native"
  make clean
  make -j${CPU_COUNT}

  cd ".."
done

cd "$BASE_DIR"

./test_archives/main.rb \
  "data/archive_urls.xz" \
  "data/valid_archives.xz" \
  "data/invalid_archives.xz" \
  "data/volatile_archives.xz"
