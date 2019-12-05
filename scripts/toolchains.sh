#!/bin/bash
set -e

cd "$(dirname $0)"

tmp="../tmp"
build="$tmp/toolchain-build"

mkdir -p "$build"
cd "$build"

if ([ -n "$CI" ] && [ -n "$TRAVIS" ]); then
  curl -s "https://codecov.io/bash" > "codecov.sh"
  chmod +x "codecov.sh"
fi

# We need to make test builds for all possible toolchains and dictionaries.
toolchains="../../cmake/toolchains"

some_test_passed=false

while read -r toolchain; do
  for dictionary in "linked-list" "sparse-array"; do
    echo "toolchain: $toolchain, dictionary: $dictionary"

    # Only special toolchain can use coverage.
    if (echo "$toolchain" | grep -q "coverage.cmake$"); then
      COVERAGE_TOOLCHAIN=true

      if ! ([ -n "$CI" ] || [ -n "$COVERAGE" ]); then
        continue
      fi
    fi

    find . \( \
      -name "CMake*" \
      -o -name "*.cmake" \
    \) -exec rm -rf {} +

    if [ "$COVERAGE_TOOLCHAIN" = true ]; then
      find . \( \
        -name "*.gcov" \
        -o -name "*.gcda" \
        -o -name "*.gcno" \
      \) -exec rm -rf {} +
    fi

    # Toolchain may not work on target platform.
    cmake "../.." \
      -DCMAKE_TOOLCHAIN_FILE="$toolchain" \
      -DLZWS_COMPRESSOR_DICTIONARY="$dictionary" \
      -DLZWS_SHARED=ON \
      -DLZWS_STATIC=ON \
      -DLZWS_CLI=OFF \
      -DLZWS_TESTS=ON \
      -DLZWS_COVERAGE=$(if [ "$COVERAGE_TOOLCHAIN" = true ]; then echo "ON"; else echo "OFF"; fi) \
      -DLZWS_EXAMPLES=ON \
      -DLZWS_MAN=OFF \
      -DCMAKE_BUILD_TYPE="RELEASE" \
      -DCMAKE_C_FLAGS_RELEASE="-O2 -march=native" \
      || continue
    make clean
    make -j2

    CTEST_OUTPUT_ON_FAILURE=1 make test

    if ([ "$COVERAGE_TOOLCHAIN" = true ] && [ -n "$CI" ] && [ -n "$TRAVIS" ]); then
      if (echo "$toolchain" | grep -q "clang/coverage.cmake$"); then
        ./codecov.sh -x "llvm-cov gcov"
      else
        ./codecov.sh -x "gcov"
      fi
    fi

    some_test_passed=true
  done
done < <(find "$toolchains" -type f)

if [ "$some_test_passed" = false ]; then
  echo "At least one test should pass" > /dev/stderr
  exit 1
fi
