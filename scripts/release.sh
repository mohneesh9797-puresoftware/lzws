#!/bin/sh
set -e

cd "$(dirname $0)"

cd "../build"

find . \( -name "CMake*" -o -name "*.cmake" \) -exec rm -rf {} +

cmake .. -DCMAKE_BUILD_TYPE="RELEASE"
make clean
make -j2 VERBOSE=1
CTEST_OUTPUT_ON_FAILURE=1 make test
make package
