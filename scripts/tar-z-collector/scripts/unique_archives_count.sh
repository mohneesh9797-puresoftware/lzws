#!/bin/bash
set -e

DIR=$(dirname "${BASH_SOURCE[0]}")
cd "$DIR"

cd ".."

valid_count=$(xzcat "data/valid_archives.xz" | cut -d " " -f 2 | sort | uniq | wc -l)
invalid_count=$(xzcat "data/invalid_archives.xz" | cut -d " " -f 2 | sort | uniq | wc -l)

echo "unique archives count: valid - ${valid_count}, invalid - ${invalid_count}"
