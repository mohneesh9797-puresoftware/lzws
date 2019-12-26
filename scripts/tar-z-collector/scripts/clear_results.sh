#!/bin/bash
set -e

DIR=$(dirname "${BASH_SOURCE[0]}")
cd "$DIR"

truncate --size=0 \
  "../data/valid_archives.xz" "../data/invalid_archives.xz" "../data/volatile_archives.xz"
