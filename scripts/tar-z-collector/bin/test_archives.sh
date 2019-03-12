#!/bin/sh
set -e

cd "$(dirname $0)"

tmp_directory="../../../tmp"

# We need to prepare release builds for all possible dictionaries.
build_dir="${tmp_directory}/linked-list-build"
mkdir -p "${build_dir}"
rm -r "${build_dir}/*"
sh -c 'cd '
# cmake "../.." -DCMAKE_BUILD_TYPE=RELEASE -DLZWS_COMPRESSOR_DICTIONARY="linked-list"


torsocks "../test_archives/main.rb" "../data/archive_urls.xz" "../data/valid_archives.xz" "../data/invalid_archives.xz" "../data/volatile_archives.xz"
