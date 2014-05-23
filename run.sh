#!/bin/sh
#
# run.sh [wine]
#
# Linux-x86_64-Clang client is launched by default. <options> are passed to the client command line.
# `NACL_SDK_ROOT` environment variable must be set to use this script.
#
# The following alternative launches are available:
#
# - `wine`: Installs the standalone Windows port into `build/Windows-test` and launches it via Wine.
#

arch=i686
[[ `uname -m` == x86_64 ]] && arch=x86_64
defaultPlatform=Linux-${arch}-Clang

case $1 in
  wine)
    cd build
    ( cd Windows-${arch} && ninja install DESTDIR=.. )

    cd OpenZone-*

    shift
    exec wine bin/Windows-${arch}/openzone.exe -p . $@
    ;;
  *)
    exec ./build/$defaultPlatform/src/tools/openzone -p . $@
    ;;
esac
