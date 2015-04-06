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

function launchWine()
{
  cd build/Windows-${1}
  cmake -DCMAKE_INSTALL_PREFIX=. -P cmake_install.cmake
  cp ../../lib/Windows-${1}/* bin

  shift
  exec wine bin/openzone.exe -p ../.. $@
}

case $1 in
  wine)
    launchWine 'i686'
    ;;
  wine64)
    launchWine 'x86_64'
    ;;
  *)
    exec ./build/$defaultPlatform/src/tools/openzone -p . $@
    ;;
esac
