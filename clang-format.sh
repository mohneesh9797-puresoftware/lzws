#!/bin/sh

clang-format -style="{ \
  BasedOnStyle: google, \
  AlignConsecutiveAssignments: true, \
  AlignConsecutiveDeclarations: true, \
  ColumnLimit: 0, \
  SpacesBeforeTrailingComments: 1\
}" \
-dump-config > .clang-format
