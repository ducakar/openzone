#!/bin/bash
#
# run.sh [wine] [<ptions>]
#
# Linux-x86_64-Clang client is launched by default. `<options>`` are passed to the client command
# line.
#
# The following alternative launches are available:
#
# - `wine`: Installs and launches standalone Windows x86_64 port via Wine.
#

set -e

defaultPlatform=Linux-x86_64-Clang-Debug

function launchWine() {
  cd build/Windows-x86_64-Debug
  cmake --install . --prefix .
  shift
  exec wine bin/openzone.exe -p ../.. "$@"
}

case $1 in
wine)
  launchWine "$@"
  ;;
*)
  exec ./build/$defaultPlatform/src/tools/openzone -p . "$@"
  ;;
esac
