#!/bin/sh
set -e

cd "$(dirname $0)"

clang-format -style="{ \
  BasedOnStyle: google, \
  AlignConsecutiveAssignments: true, \
  AlignConsecutiveDeclarations: true, \
  ColumnLimit: 0, \
  SpacesBeforeTrailingComments: 1, \
  ConstructorInitializerIndentWidth: 2, \
  ContinuationIndentWidth: 2 \
}" \
-dump-config > .clang-format
