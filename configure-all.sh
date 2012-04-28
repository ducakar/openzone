#!/bin/sh

rm -rf Linux-{x86_64,x86_64-Clang,i686,i686-Clang} Windows-i686 NaCl-{x86_64,i686}

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
