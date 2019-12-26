#!/bin/bash
set -e

DIR=$(dirname "${BASH_SOURCE[0]}")
cd "$DIR"

find "cmake" "src" \( -name "*.h" -o -name "*.c" \) -exec clang-format -i {} \;
