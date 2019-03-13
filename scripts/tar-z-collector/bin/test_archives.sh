#!/bin/sh
set -e

cd "$(dirname $0)"

tmp="../../../tmp"

# We need to create release builds for all possible dictionaries.
for dictionary in "linked-list" "sparse-array"; do
  build="${tmp}/${dictionary}-build"
  mkdir -p "${build}"
  sh -c "\
    cd '${build}' && \
    rm -rf * && \
    cmake '../..' \
      -DLZWS_COMPRESSOR_DICTIONARY='${dictionary}' \
      -DLZWS_SHARED=0 \
      -DLZWS_STATIC=1 \
      -DLZWS_CLI=1 \
      -DLZWS_TESTS=0 \
      -DCMAKE_BUILD_TYPE=RELEASE \
    && \
    make -j2\
  "
done

torsocks "../test_archives/main.rb" "../data/archive_urls.xz" "../data/valid_archives.xz" "../data/invalid_archives.xz" "../data/volatile_archives.xz"
