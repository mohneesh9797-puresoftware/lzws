#!/bin/bash
set -e

cd "$(dirname $0)"

find "cmake" "src" \( -name "*.h" -o -name "*.c" \) -exec clang-format -i {} \;
