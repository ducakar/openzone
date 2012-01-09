#!/bin/sh

if [ ! -f Linux-x86_64-Clang ]; then
  mkdir Linux-x86_64-Clang
  ( cd Linux-x86_64-Clang && cmake .. \
      -DCMAKE_C_COMPILER=/usr/bin/clang \
      -DCMAKE_CXX_COMPILER=/usr/bin/clang++ )
fi

echo ================================================================
echo
echo                        Linux-x86_64-Clang
echo
echo ----------------------------------------------------------------
( cd Linux-x86_64-Clang && time make -j4 )
echo ----------------------------------------------------------------
echo
echo                        Linux-x86_64-Clang
echo
echo ================================================================

if [ ! -f Linux-x86_64 ]; then
  mkdir Linux-x86_64
  ( cd Linux-x86_64 && cmake .. )
fi

echo ================================================================
echo
echo                           Linux-x86_64
echo
echo ----------------------------------------------------------------
( cd Linux-x86_64 && time make -j4 )
echo ----------------------------------------------------------------
echo
echo                           Linux-x86_64
echo
echo ================================================================

if [ ! -f Linux-i686-Clang ]; then
  mkdir Linux-i686-Clang
  ( cd Linux-i686-Clang && cmake .. \
      -DCMAKE_C_COMPILER=/usr/bin/clang \
      -DCMAKE_CXX_COMPILER=/usr/bin/clang++ \
      -DCMAKE_C_FLAGS=-m32 \
      -DCMAKE_CXX_FLAGS=-m32 )
fi

echo ================================================================
echo
echo                         Linux-i686-Clang
echo
echo ----------------------------------------------------------------
( cd Linux-i686-Clang && time make -j4 )
echo ----------------------------------------------------------------
echo
echo                         Linux-i686-Clang
echo
echo ================================================================

if [ ! -f Linux-i686 ]; then
  mkdir Linux-i686
  ( cd Linux-i686 && cmake .. \
      -DCMAKE_C_FLAGS=-m32 \
      -DCMAKE_CXX_FLAGS=-m32 )
fi

echo ================================================================
echo
echo                            Linux-i686
echo
echo ----------------------------------------------------------------
( cd Linux-i686 && time make -j4 )
echo ----------------------------------------------------------------
echo
echo                            Linux-i686
echo
echo ================================================================

if [ ! -f Windows-i686 ]; then
  mkdir Windows-i686
  ( cd Windows-i686 && cmake .. \
    -DCMAKE_TOOLCHAIN_FILE=../cmake/MinGW32-Toolchain.cmake )
fi

echo ================================================================
echo
echo                           Windows-i686
echo
echo ----------------------------------------------------------------
( cd Windows-i686 && time make -j4 )
echo ----------------------------------------------------------------
echo
echo                           Windows-i686
echo
echo ================================================================
