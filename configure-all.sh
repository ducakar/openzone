#!/bin/sh

Linux_x86_64=0
Linux_x86_64_Clang=1
Linux_i686=0
Linux_i686_Clang=0
Windows_i686=1
NaCl_x86_64=1
NaCl_i686=0

(( $is_build )) && return

rm -rf Linux-{x86_64,x86_64-Clang,i686,i686-Clang}
rm -rf Windows-i686
rm -rf NaCl-{x86_64,i686}

if (( $Linux_x86_64 )); then
  echo ================================================================
  echo
  echo                           Linux-x86_64
  echo
  echo ----------------------------------------------------------------

  mkdir -p Linux-x86_64
  ( cd Linux-x86_64 && cmake .. )

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

  mkdir -p Linux-x86_64-Clang
  ( cd Linux-x86_64-Clang && cmake .. \
    -DCMAKE_C_COMPILER=/usr/bin/clang \
    -DCMAKE_CXX_COMPILER=/usr/bin/clang++ )

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

  mkdir -p Linux-i686
  ( cd Linux-i686 && cmake .. \
    -DCMAKE_C_FLAGS=-m32 \
    -DCMAKE_CXX_FLAGS=-m32 )

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

  mkdir -p Linux-i686-Clang
  ( cd Linux-i686-Clang && cmake .. \
    -DCMAKE_C_COMPILER=/usr/bin/clang \
    -DCMAKE_CXX_COMPILER=/usr/bin/clang++ \
    -DCMAKE_C_FLAGS=-m32 \
    -DCMAKE_CXX_FLAGS=-m32 )

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

  mkdir -p Windows-i686
  ( cd Windows-i686 && cmake .. \
    -DCMAKE_TOOLCHAIN_FILE=../cmake/MinGW32.Toolchain.cmake )

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

  mkdir -p NaCl-x86_64
  ( cd NaCl-x86_64 && cmake .. \
    -DCMAKE_TOOLCHAIN_FILE=../cmake/NaCl-x86_64.Toolchain.cmake )

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

  mkdir -p NaCl-i686
  ( cd NaCl-i686 && cmake .. \
    -DCMAKE_TOOLCHAIN_FILE=../cmake/NaCl-i686.Toolchain.cmake )

  echo ----------------------------------------------------------------
  echo
  echo                            NaCl-i686
  echo
  echo ================================================================
  echo
fi