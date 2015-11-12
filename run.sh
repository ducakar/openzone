#!/bin/sh
#
# run.sh [wine | wine64] [<openzoneOptions>]
#
# Linux-x86_64-Clang client is launched by default. <options> are passed to the client command line.
#
# The following alternative launches are available:
#
# - `wine`: Installs and launches standalone Windows i686 port via Wine.
# - `wine64`: Installs and launches standalone Windows x86-64 port via Wine.
#

arch=i686
[[ `uname -m` == x86_64 ]] && arch=x86_64
defaultPlatform=Linux-${arch}

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
