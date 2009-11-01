#!/bin/sh

export CPP="i486-mingw32-gcc -E"
export CC="i486-mingw32-gcc"
export CXX="i486-mingw32-g++"
export LD="i486-mingw32-ld"
export LDSHARE="i486-mingw32-ld"
# must be AR="i486-mingw32-ar rc" for zlib
export AR="i486-mingw32-ar"
export RANLIB="i486-mingw32-ranlib"
export RC="i486-mingw32-windres"
export CFLAGS="-O3 -fomit-frame-pointer -march=i686 -mtune=generic -mmmx -msse -mfpmath=sse -ffast-math"
export CFLAGS="$CFLAGS -I/home/davorin/Projects/openzone/mingw32/include"
export LDFLAGS="-s -L/home/davorin/Projects/openzone/mingw32/lib"
export PREFIX="/home/davorin/Projects/openzone/mingw32"
