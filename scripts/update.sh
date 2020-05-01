#!/bin/bash -l
set -e

DIR=$(dirname "${BASH_SOURCE[0]}")
cd "$DIR"

git fetch --all || true
git fetch --tags || true
git remote | xargs -n1 -I {} git rebase "{}/$(git branch --show-current)" || true

cd "archive-collector"
rm -f "Gemfile.lock"
rvm use "."
gem install bundler
bundle update
