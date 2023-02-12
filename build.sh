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
vcpkg=0
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
  rm -rf build/{OpenZone-*,bundle}
}

function build() {
  for platform in "${platforms[@]}"; do
    header_msg "$platform-$buildType"

    if [[ "$platform" == "Emscripten" ]]; then
      toolchain_file="/usr/lib/emscripten/cmake/Modules/Platform/Emscripten.cmake"
      enable_tools=OFF
    else
      toolchain_file="$PWD/cmake/$platform.Toolchain.cmake"
      enable_tools=ON
    fi
    if [[ ! -f "$toolchain_file" ]]; then
      echo "Missing toolchain file: $platform"
      exit 1
    fi

    cmake_cmd="cmake -Wdev --warn-uninitialized -B build/$platform-$buildType -G Ninja"
    cmake_cmd="$cmake_cmd -D CMAKE_BUILD_TYPE=$buildType"

    case $platform in
    Emscripten)
      vcpkg_triplet="wasm32-emscripten"
      cmake_cmd="emcmake $cmake_cmd"
      ;;
    Windows-x86_64)
      vcpkg_triplet="x64-windows"
      ;;
    *)
      vcpkg_triplet="x64-linux"
      ;;
    esac

    if ((vcpkg)); then
      cmake_cmd="$cmake_cmd -D CMAKE_TOOLCHAIN_FILE=$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake"
      cmake_cmd="$cmake_cmd -D VCPKG_CHAINLOAD_TOOLCHAIN_FILE=$toolchain_file"
      cmake_cmd="$cmake_cmd -D VCPKG_TARGET_TRIPLET=$vcpkg_triplet"
      cmake_cmd="$cmake_cmd -D VCPKG_OVERLAY_TRIPLETS=cmake/vcpkg"
      cmake_cmd="$cmake_cmd -D OZ_TOOLS=OFF"
    else
      cmake_cmd="$cmake_cmd -D CMAKE_TOOLCHAIN_FILE=$toolchain_file"
      cmake_cmd="$cmake_cmd -D OZ_TOOLS=$enable_tools"
    fi

    (($1)) && rm -rf "build/$platform-$buildType"
    echo -e "\e[1m$cmake_cmd\e[0m"
    OZ_SOURCE_DIR=$PWD $cmake_cmd
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
