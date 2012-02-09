#!/bin/sh

echo ================================================================
echo
echo                        Linux-x86_64-Clang
echo
echo ----------------------------------------------------------------

mkdir -p Linux-x86_64-Clang
( cd Linux-x86_64-Clang && cmake .. \
  -D CMAKE_C_COMPILER=/usr/bin/clang \
  -D CMAKE_CXX_COMPILER=/usr/bin/clang++ )
( cd Linux-x86_64-Clang && time make -j4 )

echo ----------------------------------------------------------------
echo
echo                        Linux-x86_64-Clang
echo
echo ================================================================
echo
echo ================================================================
echo
echo                           Linux-x86_64
echo
echo ----------------------------------------------------------------

mkdir -p Linux-x86_64
( cd Linux-x86_64 && cmake .. )
( cd Linux-x86_64 && time make -j4 )

echo ----------------------------------------------------------------
echo
echo                           Linux-x86_64
echo
echo ================================================================
echo
echo ================================================================
echo
echo                         Linux-i686-Clang
echo
echo ----------------------------------------------------------------

mkdir -p Linux-i686-Clang
( cd Linux-i686-Clang && cmake .. \
  -D CMAKE_C_COMPILER=/usr/bin/clang \
  -D CMAKE_CXX_COMPILER=/usr/bin/clang++ \
  -D CMAKE_C_FLAGS=-m32 \
  -D CMAKE_CXX_FLAGS=-m32 )
( cd Linux-i686-Clang && time make -j4 )

echo ----------------------------------------------------------------
echo
echo                         Linux-i686-Clang
echo
echo ================================================================
echo
echo ================================================================
echo
echo                            Linux-i686
echo
echo ----------------------------------------------------------------

mkdir -p Linux-i686
( cd Linux-i686 && cmake .. \
  -D CMAKE_C_FLAGS=-m32 \
  -D CMAKE_CXX_FLAGS=-m32 )
( cd Linux-i686 && time make -j4 )

echo ----------------------------------------------------------------
echo
echo                            Linux-i686
echo
echo ================================================================
echo
echo ================================================================
echo
echo                           Windows-i686
echo
echo ----------------------------------------------------------------

mkdir -p Windows-i686
( cd Windows-i686 && cmake .. \
  -D CMAKE_TOOLCHAIN_FILE=../cmake/MinGW32-Toolchain.cmake )
( cd Windows-i686 && time make -j4 )

echo ----------------------------------------------------------------
echo
echo                           Windows-i686
echo
echo ================================================================
echo
echo ================================================================
echo
echo                           Android-i686
echo
echo ----------------------------------------------------------------

mkdir -p Android-i686
( cd Android-i686 && cmake .. \
  -D CMAKE_TOOLCHAIN_FILE=../cmake/AndroidX86-Toolchain.cmake )
( cd Android-i686 && time make -j4 )

echo ----------------------------------------------------------------
echo
echo                           Android-i686
echo
echo ================================================================
echo
echo ================================================================
echo
echo                           Android-ARM
echo
echo ----------------------------------------------------------------

mkdir -p Android-ARM
( cd Android-ARM && cmake .. \
  -D CMAKE_TOOLCHAIN_FILE=../cmake/AndroidARM-Toolchain.cmake )
( cd Android-ARM && time make -j4 )

echo ----------------------------------------------------------------
echo
echo                           Android-ARM
echo
echo ================================================================
