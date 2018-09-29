#!/bin/sh

clang-format -style="{ \
  BasedOnStyle: google, \
  AlignConsecutiveAssignments: true, \
  ColumnLimit: 0 \
}" \
-dump-config > .clang-format
