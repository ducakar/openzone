#!/bin/sh
#
# build.sh [clean | conf | build]
#
# This script configures and/or builds OpenZone for all supported platforms in the `build`
# directory. The following commands may be given (`build` is assumed if none):
#
# - `clean`: Delete all builds.
# - `conf`: Delete all builds and configure (but not build) them anew.
# - `build`: Configure (if necessary) and build all builds.
#

buildType=Debug
platforms=(
  Linux-x86_64 Linux-x86_64-Clang Linux-i686 Linux-i686-Clang
  Windows-i686
  NaCl-x86_64 NaCl-i686 PNaCl
  #Android14-i686 Android14-ARM Android14-ARMv7a Android14-MIPS
)

# Extract path to PNaCl SDK from CMake toolchain file.
pnaclRoot=`egrep '^set\( PLATFORM_PREFIX' cmake/PNaCl.Toolchain.cmake | \
          sed -r 's|^set\( PLATFORM_PREFIX *"(.*)\" \)|\1|'`

function clean()
{
  for platform in ${platforms[@]}; do
    rm -rf build/$platform
  done
  rm -rf build/{OpenZone-*,NaCl-test,Android}
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
        -D CMAKE_BUILD_TYPE=$buildType \
        -D CMAKE_TOOLCHAIN_FILE=../../cmake/$platform.Toolchain.cmake \
        ../.. )
    fi
    (( $1 )) || ( cd build/$platform && time make -j4 )

    echo ----------------------------------------------------------------
    echo
    echo                           $platform
    echo
    echo ================================================================
    echo
  done
}

function pnacl()
{
  for arch in x86_64 i686 arm; do
    echo "Building openzone.$arch.nexe"
    "$pnaclRoot/bin64/pnacl-translate" -arch $arch -o build/PNaCl/src/tools/openzone.$arch.nexe \
                                       build/PNaCl/src/tools/openzone.pexe
  done
}

case $1 in
  clean)
    clean
    ;;
  conf)
    build 1
    ;;
  pnacl)
    pnacl
    ;;
  build|*)
    build 0
    ;;
esac
