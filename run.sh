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

nacl86Prefix="$NACL_SDK_ROOT/toolchain/linux_x86_newlib"
naclARMPrefix="$NACL_SDK_ROOT/toolchain/linux_arm_newlib"
pnaclPrefix="$NACL_SDK_ROOT/toolchain/linux_x86_pnacl/newlib"

function run_nacl()
{
  mkdir -p build/NaCl-test

  # Just create symlinks instead of copying.
  for i in share/openzone/*.{7z,zip} share/openzone/packages.ozManifest \
           build/NaCl-*/src/tools/openzone.*.nexe build/PNaCl/src/tools/openzone.pexe \
           etc/nacl/openzone.nmf etc/nacl/openzone.??.html doc
  do
    [[ -e $i ]] && ln -sf ../../$i build/NaCl-test
  done

  # Strip binaries if `strip` option is given.
  if [[ $arg == strip ]]; then
    if [[ -e build/NaCl-test/openzone.x86_64.nexe ]]; then
      "$nacl86Prefix/bin/x86_64-nacl-strip" build/NaCl-test/openzone.x86_64.nexe
    fi
    if [[ -e build/NaCl-test/openzone.i686.nexe ]]; then
      "$nacl86Prefix/bin/i686-nacl-strip" build/NaCl-test/openzone.i686.nexe
    fi
    if [[ -e build/NaCl-test/openzone.ARM.nexe ]]; then
      "$naclARMPrefix/bin/arm-nacl-strip" build/NaCl-test/openzone.ARM.nexe
    fi
    if [[ -e build/NaCl-test/openzone.pexe ]]; then
      "$pnaclPrefix/bin64/pnacl-strip" build/NaCl-test/openzone.pexe
    fi
  fi

  cd build/NaCl-test
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
