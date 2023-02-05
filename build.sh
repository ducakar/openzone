#!/bin/bash
#
# build.sh [clean | conf] [<platform>]
#
# This script configures and/or builds OpenZone in the `build` directory for a specified platform or
# all supported platforms uncommented in the beginning of this script if `<platform>` parameter is
# omitted. `NACL_SDK_ROOT` environment variable must be set for the NaCl build.
#
# The following commands may be given:
#
# - `clean`: Delete build(s).
# - `conf`: Delete build(s) and configure (but not build) them anew.
# - (none): Configure (if necessary) and build.

set -e

buildType=Debug
platforms=(
  # Emscripten
  Linux-x86_64
  Linux-x86_64-Clang
  # PNaCl
  # Windows-x86_64
)

. etc/common.sh

function clean() {
  for platform in "${platforms[@]}"; do
    rm -rf "build/$platform-$buildType"
  done
  rm -rf build/{OpenZone-*,NaCl-test,Windows-test,bundle}
}

function build() {
  for platform in "${platforms[@]}"; do
    if [[ "$platform" != "Emscripten" && ! -f "cmake/$platform.Toolchain.cmake" ]]; then
      echo "Unknown platform: $platform"
      continue
    fi

    header_msg "$platform-$buildType"

    (($1)) && rm -rf "build/$platform-$buildType"
    if [[ ! -d "build/$platform-$buildType" ]]; then
      mkdir -p "build/$platform-$buildType"
      if [[ "$platform" == "Emscripten" ]]; then
        emcmake cmake -B "build/$platform-$buildType" -Wdev --warn-uninitialized \
          -G Ninja \
          -D CMAKE_BUILD_TYPE="$buildType"
      else
        cmake -B "build/$platform-$buildType" -Wdev --warn-uninitialized \
          -G Ninja \
          -D CMAKE_TOOLCHAIN_FILE="../../cmake/$platform.Toolchain.cmake" \
          -D CMAKE_BUILD_TYPE="$buildType" \
          -D OZ_TOOLS=ON
      fi
    fi
    (($1)) || time cmake --build "build/$platform-$buildType"
  done
}

case $1 in
clean)
  if [[ -n $2 ]]; then platforms=("$2"); fi
  clean
  ;;
conf)
  if [[ -n $2 ]]; then platforms=("$2"); fi
  build 1
  ;;
*)
  if [[ -n $1 ]]; then platforms=("$1"); fi
  build 0
  ;;
esac
