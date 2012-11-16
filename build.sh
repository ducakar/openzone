#!/bin/sh

buildType=Debug
platforms=( \
  Linux-x86_64 Linux-x86_64-Clang Linux-i686 Linux-i686-Clang \
  Windows-i686 \
  NaCl-x86_64 NaCl-i686 #PNaCl \
  Android14-i686 Android14-ARM Android14-ARMv7a Android14-MIPS \
)

function clean()
{
  for platform in ${platforms[@]}; do
    rm -rf build/$platform
  done
  rm -rf build/{NaCl-test,Android}
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

case $1 in
  clean)
    clean
    ;;
  conf)
    build 1
    ;;
  build|*)
    build 0
    ;;
esac
