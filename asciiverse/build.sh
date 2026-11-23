#!/bin/sh
# Builds Matanza: the game server, matanza-server, tangled from Alejandro
# Forero Cuervo's CWEB source matanza.w with ctangle and built with its own
# configure, and the launcher, matanza (Rust, in launcher/), which plays,
# hosts over SSH and joins. Both land in build/$PLATFORM/.
#
#   PLATFORM=linux sh asciiverse/build.sh
#
# Optional: CC, CFLAGS, LDFLAGS for the server, CARGO_TARGET for the
# launcher, STRIP.
set -eu
cd "$(dirname "$0")/.."
PLATFORM=${PLATFORM:-linux}
CC=${CC:-cc}
CFLAGS=${CFLAGS:--O2 -std=gnu99}
LDFLAGS=${LDFLAGS:-}
STRIP=${STRIP:-strip}
export TERM="${TERM:-xterm}"
out=build/$PLATFORM
src=build/server-$PLATFORM
rm -rf "$out" "$src"
mkdir -p "$out" "$src"

# the server, built in a copy of the tracked files so the tree stays clean
if git rev-parse --git-dir >/dev/null 2>&1; then
  git ls-files -- . ':!launcher' ':!asciiverse' ':!.github' | tar -cf - -T - | tar -xf - -C "$src"
else
  tar -cf - --exclude=./build --exclude=./launcher --exclude=./asciiverse . | tar -xf - -C "$src"
fi
(
  cd "$src"
  # PNG backgrounds are an option of the server's own; the game needs none,
  # so it is built without libpng and depends only on the C library.
  # (an autoconf 2.13 configure: it takes CC and the flags from the
  # environment, not as arguments)
  ac_cv_header_png_h=no ac_cv_lib_png_png_sig_cmp=no CC="$CC" CFLAGS="$CFLAGS" LDFLAGS="$LDFLAGS" \
    ./configure >configure.out 2>&1 \
    || { tail -30 configure.out; exit 1; }
  make matanza CC="$CC" CFLAGS="$CFLAGS" LDFLAGS="$LDFLAGS" >make.out 2>&1 || { tail -40 make.out; exit 1; }
)
cp "$src/matanza" "$out/matanza-server"
$STRIP "$out/matanza-server" 2>/dev/null || true

# the launcher
if [ -n "${CARGO_TARGET:-}" ]; then
  cargo build --release --locked --manifest-path launcher/Cargo.toml --target "$CARGO_TARGET"
  cp "launcher/target/$CARGO_TARGET/release/matanza" "$out/matanza"
else
  cargo build --release --locked --manifest-path launcher/Cargo.toml
  cp launcher/target/release/matanza "$out/matanza"
fi
ls -l "$out"
