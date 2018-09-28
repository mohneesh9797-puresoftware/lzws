#!/bin/sh

clang-format -style="{ \
  BasedOnStyle: google, \
  ColumnLimit: 0 \
}" \
-dump-config > .clang-format
