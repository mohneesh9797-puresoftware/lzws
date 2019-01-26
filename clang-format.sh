#!/bin/sh
set -e

cd "$(dirname $0)"

find "cmake" "src" "tests" -name "*.h" -o -name "*.c" -exec clang-format -i {} \;
