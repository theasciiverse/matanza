#!/bin/sh
# Packs build/$PLATFORM into dist/matanza-<target>.tar.gz with the
# licence, README and PATCHES, the way av installs it.
#
#   sh asciiverse/package.sh <target-triple> <build folder>
set -eu
cd "$(dirname "$0")/.."
target=$1
from=$2
stage=stage/matanza-$target
rm -rf "$stage"
mkdir -p "$stage" dist
cp "$from/matanza" "$from/matanza-server" "$stage/"
cp COPYING "$stage/LICENSE"
cp README.md PATCHES.md "$stage/"
tar -czf "dist/matanza-$target.tar.gz" -C "$stage" .
echo "dist/matanza-$target.tar.gz"
