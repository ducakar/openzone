#!/bin/sh

prefix="`pwd`/../openzone-build/mingw32"
triplet="i486-mingw32"
triplet_minus="${triplet}-"

export LC_ALL=C

export CPP="${triplet_minus}gcc -E"
export CC="${triplet_minus}gcc"
export CXX="${triplet_minus}g++"
export LD="${triplet_minus}ld"
export LDSHARE="${triplet_minus}ld"
# must be AR="${triplet_minus}ar rc" for zlib
export AR="${triplet_minus}ar"
export STRIP="${triplet_minus}strip"
export RANLIB="${triplet_minus}ranlib"
export RC="${triplet_minus}windres"
export CFLAGS="-O2 -march=i686 -mtune=generic"
export CFLAGS="$CFLAGS -I${prefix}/include"
export LDFLAGS="-s -L${prefix}/lib"
