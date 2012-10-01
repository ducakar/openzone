#!/bin/sh

buildType=Debug

Linux_x86_64=1
Linux_x86_64_Clang=1
Linux_i686=1
Linux_i686_Clang=1
Windows_i686=1
NaCl_x86_64=1
NaCl_i686=1
Android_i686=0
Android_ARMv7a=0

(( $isBuild )) && return

rm -rf build

if (( $Linux_x86_64 )); then
  echo ================================================================
  echo
  echo                           Linux-x86_64
  echo
  echo ----------------------------------------------------------------

  mkdir -p build/Linux-x86_64
  ( cd build/Linux-x86_64 && cmake ../.. \
    -DCMAKE_BUILD_TYPE=$buildType \
    -DCMAKE_TOOLCHAIN_FILE=../../cmake/Linux-x86_64.Toolchain.cmake )

  echo ----------------------------------------------------------------
  echo
  echo                           Linux-x86_64
  echo
  echo ================================================================
  echo
fi

if (( $Linux_x86_64_Clang )); then
  echo ================================================================
  echo
  echo                        Linux-x86_64-Clang
  echo
  echo ----------------------------------------------------------------

  mkdir -p build/Linux-x86_64-Clang
  ( cd build/Linux-x86_64-Clang && cmake ../.. \
    -DCMAKE_BUILD_TYPE=$buildType \
    -DCMAKE_TOOLCHAIN_FILE=../../cmake/Linux-x86_64-Clang.Toolchain.cmake )

  echo ----------------------------------------------------------------
  echo
  echo                        Linux-x86_64-Clang
  echo
  echo ================================================================
  echo
fi

if (( $Linux_i686 )); then
  echo ================================================================
  echo
  echo                            Linux-i686
  echo
  echo ----------------------------------------------------------------

  mkdir -p build/Linux-i686
  ( cd build/Linux-i686 && cmake ../.. \
    -DCMAKE_BUILD_TYPE=$buildType \
    -DCMAKE_TOOLCHAIN_FILE=../../cmake/Linux-i686.Toolchain.cmake )

  echo ----------------------------------------------------------------
  echo
  echo                            Linux-i686
  echo
  echo ================================================================
  echo
fi

if (( $Linux_i686_Clang )); then
  echo ================================================================
  echo
  echo                         Linux-i686-Clang
  echo
  echo ----------------------------------------------------------------

  mkdir -p build/Linux-i686-Clang
  ( cd build/Linux-i686-Clang && cmake ../.. \
    -DCMAKE_BUILD_TYPE=$buildType \
    -DCMAKE_TOOLCHAIN_FILE=../../cmake/Linux-i686-Clang.Toolchain.cmake )

  echo ----------------------------------------------------------------
  echo
  echo                         Linux-i686-Clang
  echo
  echo ================================================================
  echo
fi

if (( $Windows_i686 )); then
  echo ================================================================
  echo
  echo                           Windows-i686
  echo
  echo ----------------------------------------------------------------

  mkdir -p build/Windows-i686
  ( cd build/Windows-i686 && cmake ../.. \
    -DCMAKE_BUILD_TYPE=$buildType \
    -DCMAKE_TOOLCHAIN_FILE=../../cmake/Windows-i686.Toolchain.cmake )

  echo ----------------------------------------------------------------
  echo
  echo                           Windows-i686
  echo
  echo ================================================================
  echo
fi

if (( $NaCl_x86_64 )); then
  echo ================================================================
  echo
  echo                           NaCl-x86_64
  echo
  echo ----------------------------------------------------------------

  mkdir -p build/NaCl-x86_64
  ( cd build/NaCl-x86_64 && cmake ../.. \
    -DCMAKE_BUILD_TYPE=$buildType \
    -DCMAKE_TOOLCHAIN_FILE=../../cmake/NaCl-x86_64.Toolchain.cmake )

  echo ----------------------------------------------------------------
  echo
  echo                           NaCl-x86_64
  echo
  echo ================================================================
  echo
fi

if (( $NaCl_i686 )); then
  echo ================================================================
  echo
  echo                            NaCl-i686
  echo
  echo ----------------------------------------------------------------

  mkdir -p build/NaCl-i686
  ( cd build/NaCl-i686 && cmake ../.. \
    -DCMAKE_BUILD_TYPE=$buildType \
    -DCMAKE_TOOLCHAIN_FILE=../../cmake/NaCl-i686.Toolchain.cmake )

  echo ----------------------------------------------------------------
  echo
  echo                            NaCl-i686
  echo
  echo ================================================================
  echo
fi

if (( $Android_i686 )); then
  echo ================================================================
  echo
  echo                           Android-i686
  echo
  echo ----------------------------------------------------------------

  mkdir -p build/Android-i686
  ( cd build/Android-i686 && cmake ../.. \
    -DCMAKE_BUILD_TYPE=$buildType \
    -DCMAKE_TOOLCHAIN_FILE=../../cmake/Android-i686.Toolchain.cmake )

  echo ----------------------------------------------------------------
  echo
  echo                           Android-i686
  echo
  echo ================================================================
  echo
fi

if (( $Android_ARMv7a )); then
  echo ================================================================
  echo
  echo                          Android-ARMv7a
  echo
  echo ----------------------------------------------------------------

  mkdir -p build/Android-ARMv7a
  ( cd build/Android-ARMv7a && cmake ../.. \
    -DCMAKE_BUILD_TYPE=$buildType \
    -DCMAKE_TOOLCHAIN_FILE=../../cmake/Android-ARMv7a.Toolchain.cmake )

  echo ----------------------------------------------------------------
  echo
  echo                          Android-ARMv7a
  echo
  echo ================================================================
  echo
fi
