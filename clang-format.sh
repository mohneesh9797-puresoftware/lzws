#!/bin/sh

clang-format -style="{ \
  BasedOnStyle: google, \
  AlignConsecutiveAssignments: true, \
  AlignConsecutiveDeclarations: true, \
  ColumnLimit: 0 \
}" \
-dump-config > .clang-format
