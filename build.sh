#!/bin/sh
#
# build.sh [clean | conf]
#
# This script configures and/or builds OpenZone in the `build` directory for all platforms that are
# uncommented in the beginning of this script. `ANDROID_NDK` and `NACL_SDK_ROOT` environment
# variables must be set for Android and NaCl builds.
#
# The following commands may be given:
#
# - `clean`: Delete all builds.
# - `conf`: Delete all builds and configure (but not build) them anew.
# - `pnacl`: Run `pnacl-translate` to convert client `.pexe` to platform-dependent `.nexe`s.
# - (none): Configure (if necessary) and build all enabled builds.

buildType=Debug
platforms=(
  Linux-`uname -m`-Clang
#   Linux-x86_64
#   Linux-x86_64-Clang
#   Linux-i686
#   Linux-i686-Clang
#   Windows-i686
  PNaCl
#   Android14-i686
#   Android14-ARM
#   Android14-ARMv7a
#   Android14-MIPS
#   Emscripten
)

function clean()
{
  for platform in ${platforms[@]}; do
    rm -rf build/$platform
  done
  rm -rf build/{OpenZone-*,NaCl-test,Windows-test,Android,bundle}
}

function build()
{
  for platform in ${platforms[@]}; do
    echo ================================================================
    echo
    echo                           $platform
    echo
    echo ----------------------------------------------------------------

    (( $1 )) && rm -rf build/$platform
    if [[ ! -d build/$platform ]]; then
      mkdir -p build/$platform
      ( cd build/$platform && cmake -Wdev --warn-uninitialized \
        -D CMAKE_TOOLCHAIN_FILE=../../cmake/$platform.Toolchain.cmake \
        -D CMAKE_BUILD_TYPE=$buildType \
        ../.. )
    fi
    (( $1 )) || ( cd build/$platform && time make -j`nproc` )

    echo ----------------------------------------------------------------
    echo
    echo                           $platform
    echo
    echo ================================================================
    echo
  done
}

case $1 in
  clean)
    clean
    ;;
  conf)
    build 1
    ;;
  *)
    build 0
    ;;
esac
