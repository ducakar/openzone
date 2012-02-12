#!/bin/sh

echo ================================================================
echo
echo                        Linux-x86_64-Clang
echo
echo ----------------------------------------------------------------

mkdir -p Linux-x86_64-Clang
( cd Linux-x86_64-Clang && cmake .. \
  -DCMAKE_C_COMPILER=/usr/bin/clang \
  -DCMAKE_CXX_COMPILER=/usr/bin/clang++ )
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
  -DCMAKE_C_COMPILER=/usr/bin/clang \
  -DCMAKE_CXX_COMPILER=/usr/bin/clang++ \
  -DCMAKE_C_FLAGS=-m32 \
  -DCMAKE_CXX_FLAGS=-m32 )
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
  -DCMAKE_C_FLAGS=-m32 \
  -DCMAKE_CXX_FLAGS=-m32 )
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
  -DCMAKE_TOOLCHAIN_FILE=../cmake/mingw32.toolchain.cmake )
( cd Windows-i686 && time make -j4 )

echo ----------------------------------------------------------------
echo
echo                           Windows-i686
echo
echo ================================================================
# echo
# echo ================================================================
# echo
# echo                           Android-i686
# echo
# echo ----------------------------------------------------------------
#
# mkdir -p Android-i686
# ( cd Android-i686 && cmake .. \
#   -DCMAKE_TOOLCHAIN_FILE=../cmake/android-x86.toolchain.cmake )
# ( cd Android-i686 && time make -j4 )
#
# echo ----------------------------------------------------------------
# echo
# echo                           Android-i686
# echo
# echo ================================================================
# echo
# echo ================================================================
# echo
# echo                           Android-ARM
# echo
# echo ----------------------------------------------------------------
#
# mkdir -p Android-ARM
# ( cd Android-ARM && cmake .. \
#   -DCMAKE_TOOLCHAIN_FILE=../cmake/android-arm.toolchain.cmake )
# ( cd Android-ARM && time make -j4 )
#
# echo ----------------------------------------------------------------
# echo
# echo                           Android-ARM
# echo
# echo ================================================================
