#!/bin/sh
#
# run.sh [win | nacl] [<options>]
#
# Linux-x86_64-Clang client is launched by default. <options> are passed to the client command line.
# `NACL_SDK_ROOT` environment variable must be set to use this script.
#
# The following alternative launches are available:
#
# - `wine`: Installs the standalone Windows port into `build/Windows-test` and launches it via Wine.
# - `nacl`: Prepares directory `build/NaCl-test` (creating symlinks to binaries, compiled data
#   archives, HTML pages ...), starts a simple python web server in that directory at port 8000
#   (`python -m http.server`) and opens `localhost:8000` in chromium browser to test the web-based
#   NaCl port. If `strip` is passed as the second option, binaries are stripped too.
#

defaultPlatform=Linux-`uname -m`-Clang

function run_nacl()
{
  mkdir -p build/PNaCl/src/tools

  # Just create symlinks instead of copying.
  for i in share/openzone/*.{7z,zip} share/openzone/packages.ozManifest \
           etc/nacl/openzone.nmf etc/nacl/openzone.??.html doc
  do
    [[ -e $i ]] && ln -sf ../../../../$i build/PNaCl/src/tools
  done

  cd build/PNaCl/src/tools
  python -m http.server &
  serverPID=$!

  sleep 3
  chromium --user-data-dir="$HOME/.config/chromium-test" \
           http://localhost:8000/openzone.sl.html || true

  kill $serverPID
}

cmd=$1
arg=$2

case $1 in
  wine)
    cd build
    ( cd Windows-i686 && make -j4 install DESTDIR=.. )

    cd OpenZone-*

    shift
    exec wine bin/Windows-i686/openzone.exe -p . $@
    ;;
  nacl)
    run_nacl
    ;;
  *)
    exec ./build/$defaultPlatform/src/tools/openzone -p . $@
    ;;
esac
