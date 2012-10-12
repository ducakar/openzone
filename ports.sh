#!/bin/sh

platforms=( \
#   Linux-x86_64 Linux-x86_64-Clang Linux-i686 Linux-i686-Clang \
#   Windows-i686 \
  NaCl-x86_64 NaCl-i686 \
#   Android9-i686 Android9-ARM Android9-MIPS \
)

projectDir=`pwd`
topDir="$projectDir/ports"
naclPrefix="/home/davorin/Projects/nacl_sdk/pepper_22/toolchain/linux_x86_newlib"

function setup_nacl64()
{
  buildDir="$topDir/NaCl-x86_64"
  sysroot="$naclPrefix/x86_64-nacl"
  toolchain="$projectDir/cmake/NaCl-x86_64.Toolchain.cmake"

  export CC="$naclPrefix/bin/x86_64-nacl-gcc"
  export CXX="$naclPrefix/bin/x86_64-nacl-g++"
  export CFLAGS="-march=x86-64 -msse3 -O3"
  export CXXFLAGS="-march=x86-64 -msse3 -O3"
  export AR="$naclPrefix/bin/x86_64-nacl-ar"
  export RANLIB="$naclPrefix/bin/x86_64-nacl-ranlib"
  export LD="$naclPrefix/bin/x86_64-nacl-ld"
}

function setup_nacl32()
{
  buildDir="$topDir/NaCl-i686"
  sysroot="$naclPrefix/i686-nacl"
  toolchain="$projectDir/cmake/NaCl-i686.Toolchain.cmake"

  export CC="$naclPrefix/bin/i686-nacl-gcc"
  export CXX="$naclPrefix/bin/i686-nacl-g++"
  export CFLAGS="-march=i686 -msse3 -mfpmath=sse -O3"
  export CXXFLAGS="-march=i686 -msse3 -mfpmath=sse -O3"
  export AR="$naclPrefix/bin/i686-nacl-ar"
  export RANLIB="$naclPrefix/bin/i686-nacl-ranlib"
  export LD="$naclPrefix/bin/i686-nacl-ld"
}

function clean()
{
  for platform in ${platforms[@]}; do
    rm -rf "$topDir/$platform"
  done
}

function download()
{
  mkdir -p "$topDir/archives"
  wget -c -nc -P "$topDir/archives" $1
}

function fetch()
{
  # zlib
  download 'http://zlib.net/zlib-1.2.7.tar.bz2'

  # physfs
  download 'http://icculus.org/physfs/downloads/physfs-2.0.2.tar.gz'
}

function unpack()
{
  mkdir -p "$buildDir"
  tar xf "$topDir/archives/$1" -C "$buildDir"
}

function build_zlib()
{
  name="zlib-1.2.7"

  [[ -d "$buildDir/$name" ]] && return

  unpack "$name.tar.bz2"
  cd "$buildDir/$name"

  ./configure --prefix=/usr
  make -j4
  make install DESTDIR="$buildDir"
}

function build_physfs()
{
  name="physfs-2.0.2"

  [[ -d "$buildDir/$name" ]] && return

  unpack "$name.tar.gz"
  cd "$buildDir/$name"

  patch -p1 < "$projectDir/etc/nacl-physfs-2.0.2.patch"
  mkdir -p build && cd build
  cmake \
    -D CMAKE_TOOLCHAIN_FILE="$toolchain" \
    -D CMAKE_INSTALL_PREFIX="/usr" \
    -D CMAKE_BUILD_TYPE="Release" \
    -D PHYSFS_BUILD_SHARED=0 \
    -D PHYSFS_BUILD_TEST=0 \
    ..
  make -j4
  make install DESTDIR="$buildDir"
}

function build()
{
  #
  # zlib
  #
  setup_nacl64
  export CFLAGS="$CFLAGS -Dunlink=puts"
  build_zlib
  rm -rf "$buildDir"/usr/lib/*.so*

  setup_nacl32
  export CFLAGS="$CFLAGS -Dunlink=puts"
  build_zlib
  rm -rf "$buildDir"/usr/lib/*.so*

  #
  # physfs
  #
  setup_nacl64
  build_physfs "$projectDir/etc/nacl-physfs-2.0.2.patch"

  setup_nacl32
  build_physfs "$projectDir/etc/nacl-physfs-2.0.2.patch"
}

case "$1" in
  clean)
    clean
    ;;
  fetch)
    fetch
    ;;
  *)
    build
    ;;
esac
