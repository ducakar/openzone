#!/bin/sh
#
# ports.sh [clean | fetch | build]
#
# This script is used to build libraries required by OpenZone for some platforms. Currently it
# builds all required libraries for NaCl and Android configurations that are not provided by SDKs.
# `ANDROID_NDK` and `NACL_SDK_ROOT` environment variables must be set to use this script.
#
# The following commands may be given (`build` is assumed if none):
#
# - `clean`: Delete everything in `ports` directory except downloaded sources.
# - `buildclean`: Delete build directories. Downloaded sources and installed libraries are left
#   intact.
# - `fetch`: Download sources into `ports/archives` directory.
# - (none): Builds all libraries for all platforms.
#

platforms=(
#  Android14-i686
#  Android14-ARMv7a
  Emscripten
)

buildTriplet="`uname -m`-`uname -i`-linux-gnu"

projectDir=`pwd`
topDir="$projectDir/ports"
originalPath="$PATH"

ndkX86Tools="$ANDROID_NDK/toolchains/x86-4.9/prebuilt/linux-x86_64"
ndkX86Platform="$ANDROID_NDK/platforms/android-14/arch-x86"

ndkARMTools="$ANDROID_NDK/toolchains/arm-linux-androideabi-4.9/prebuilt/linux-x86_64"
ndkARMPlatform="$ANDROID_NDK/platforms/android-14/arch-arm"

. etc/common.sh

function setup_ndk_i686()
{
  platform="Android14-i686"                               # Platform name.

  buildDir="$topDir/$platform"                            # Build and install directory.
  triplet="i686-linux-android"                            # Platform triplet (tools prefix).
  hostTriplet="$triplet"                                  # Host triplet for autotools configure.
  toolsroot="$ndkX86Tools"                                # SDK tool root.
  toolchain="$projectDir/cmake/$platform.Toolchain.cmake" # CMake toolchain.
  sysroot="$ndkX86Platform"                               # SDK sysroot.

  export CPP="$toolsroot/bin/$triplet-cpp"
  export CC="$toolsroot/bin/$triplet-gcc"
  export CXX="$toolsroot/bin/$triplet-g++"
  export AR="$toolsroot/bin/$triplet-ar"
  export RANLIB="$toolsroot/bin/$triplet-ranlib"
  export STRIP="$toolsroot/bin/$triplet-strip"
  export PKG_CONFIG_PATH="$buildDir/usr/lib/pkgconfig"
  export PKG_CONFIG_LIBDIR="$buildDir/usr/lib"
  export PATH="$toolsroot/bin:$originalPath"

  export CPPFLAGS="--sysroot=$sysroot -isystem $buildDir/usr/include"
  export CFLAGS="-Ofast -fPIC -march=i686 -msse3 -mfpmath=sse"
  export CXXFLAGS="-Ofast -fPIC -march=i686 -msse3 -mfpmath=sse"
  export LDFLAGS="--sysroot=$sysroot -L$buildDir/usr/lib"

  for p in ${platforms[@]}; do
    [[ $p == $platform ]] && return 0
  done
  return 1
}

function setup_ndk_ARMv7a()
{
  platform="Android14-ARMv7a"                             # Platform name.
  buildDir="$topDir/$platform"                            # Build and install directory.
  triplet="arm-linux-androideabi"                         # Platform triplet (tools prefix).
  hostTriplet="$triplet"                                  # Host triplet for autotools configure.
  toolsroot="$ndkARMTools"                                # SDK tool root.
  toolchain="$projectDir/cmake/$platform.Toolchain.cmake" # CMake toolchain.
  sysroot="$ndkARMPlatform"                               # SDK sysroot.

  export CPP="$toolsroot/bin/$triplet-cpp"
  export CC="$toolsroot/bin/$triplet-gcc"
  export CXX="$toolsroot/bin/$triplet-g++"
  export AR="$toolsroot/bin/$triplet-ar"
  export RANLIB="$toolsroot/bin/$triplet-ranlib"
  export STRIP="$toolsroot/bin/$triplet-strip"
  export PKG_CONFIG_PATH="$buildDir/usr/lib/pkgconfig"
  export PKG_CONFIG_LIBDIR="$buildDir/usr/lib"
  export PATH="$toolsroot/bin:$originalPath"

  export CPPFLAGS="--sysroot=$sysroot -isystem $buildDir/usr/include"
  export CFLAGS="-Ofast -fPIC -march=armv7-a -mfloat-abi=softfp -mfpu=neon -Wno-psabi"
  export CXXFLAGS="-Ofast -fPIC -march=armv7-a -mfloat-abi=softfp -mfpu=neon -Wno-psabi"
  export LDFLAGS="--sysroot=$sysroot -L$buildDir/usr/lib -Wl,--fix-cortex-a8"

  for p in ${platforms[@]}; do
    [[ $p == $platform ]] && return 0
  done
  return 1
}

function setup_emscripten()
{
  platform="Emscripten"
  buildDir="$topDir/$platform"

  export CPP=""
  export CC=""
  export CXX=""
  export AR=""
  export RANLIB=""
  export STRIP=""
  export PKG_CONFIG_PATH="$buildDir/usr/lib/pkgconfig"
  export PKG_CONFIG_LIBDIR="$buildDir/usr/lib"
  export PATH="$originalPath"

  export CPPFLAGS=""
  export CFLAGS="-O3"
  export CXXFLAGS="-O3"
  export LDFLAGS=""

  for p in ${platforms[@]}; do
    [[ $p == $platform ]] && return 0
  done
  return 1
}

function clean()
{
  for platform in ${platforms[@]}; do
    rm -rf "$topDir/$platform"
  done
}

function buildclean()
{
  for platform in ${platforms[@]}; do
    for subDir in `echo "$topDir/$platform/*"`; do
      [[ $subDir == */usr ]] || rm -rf "$subDir"
    done

    rm -rf "$topDir/$platform"/usr/{bin,doc,man,share}
    rm -rf "$topDir/$platform"/usr/lib/{libpng,lua}
    rm -rf "$topDir/$platform"/usr/lib/*.la
  done
}

function download()
{
  mkdir -p "$topDir/archives"
  wget -c -nc -P "$topDir/archives" $@
}

function fetch()
{
  # zlib
  download 'http://zlib.net/zlib-1.2.8.tar.xz'

  # libpng
  download 'http://downloads.sourceforge.net/sourceforge/libpng/libpng-1.6.16.tar.xz'

  # jpeglib
  download 'http://www.ijg.org/files/jpegsrc.v9a.tar.gz'

  # libogg
  download 'http://downloads.xiph.org/releases/ogg/libogg-1.3.2.tar.xz'

  # libvorbis
  download 'http://downloads.xiph.org/releases/vorbis/libvorbis-1.3.5.tar.xz'

  # FreeType
  download 'http://sourceforge.net/projects/freetype/files/freetype2/2.5.5/freetype-2.5.5.tar.bz2'

  # PhysicsFS
  download 'http://icculus.org/physfs/downloads/physfs-2.0.3.tar.bz2'

  # Lua
  download 'http://www.lua.org/ftp/lua-5.3.1.tar.gz'

  # OpenAL Soft
  download 'http://kcat.strangesoft.net/openal-releases/openal-soft-1.16.0.tar.bz2'

  # SDL2
  download 'http://www.libsdl.org/release/SDL2-2.0.3.tar.gz'

  # SDL2_ttf
  download 'http://www.libsdl.org/projects/SDL_ttf/release/SDL2_ttf-2.0.12.tar.gz'
}

function prepare()
{
  [[ -d "$buildDir/$1" ]] && return 1

  header_msg "$1 @ $platform"

  mkdir -p "$buildDir"
  if [[ -d "$topDir/archives/$2" ]]; then
    cp -R "$topDir/archives/$2" "$buildDir"
  else
    tar xf "$topDir/archives/$2" -C "$buildDir"
  fi

  cd "$buildDir/$1"
}

function applyPatches()
{
  for patchFile in $@; do
    patch -p1 < "$projectDir/etc/patches/$patchFile" || exit 1
  done
}

function cmakeBuild()
{
  mkdir -p build && cd build

  if [[ $platform == Emscripten ]]; then
    emcmake cmake \
      -G Ninja \
      -D CMAKE_MODULE_PATH="$projectDir/cmake" \
      -D CMAKE_BUILD_TYPE="Release" \
      -D CMAKE_INSTALL_PREFIX="/usr" \
      $@ \
      .. || return 1
  else
    cmake \
      -G Ninja \
      -D CMAKE_MODULE_PATH="$projectDir/cmake" \
      -D CMAKE_TOOLCHAIN_FILE="$toolchain" \
      -D CMAKE_BUILD_TYPE="Release" \
      -D CMAKE_INSTALL_PREFIX="/usr" \
      -D PLATFORM_PORTS_PREFIX="$buildDir" \
      $@ \
      .. || return 1
  fi

  ninja || exit 1
  cmake -DCMAKE_INSTALL_PREFIX="$buildDir/usr" -P cmake_install.cmake
}

function autotoolsBuild()
{
  if [[ $platform == Emscripten ]]; then
    emconfigure ./configure --prefix=/usr $@ || exit 1
  else
    ./configure --build=$buildTriplet --host=$hostTriplet --prefix=/usr $@ || exit 1
  fi

  make -j4 || exit 1
  make install DESTDIR="$buildDir"
}

function finish()
{
  [[ -d "$buildDir"/usr/lib/pkgconfig ]] || return

  # Fix paths in pkg-config files.
  for file in "$buildDir"/usr/lib/pkgconfig/*.pc; do
    sed -r 's|=/usr|='"$buildDir"'/usr|g' -i $file
  done
}

function build_zlib()
{
  prepare zlib-1.2.8 zlib-1.2.8.tar.xz || return

  autotoolsBuild --static

  rm -rf "$buildDir"/usr/lib/libz.so*

  finish
}

function build_libpng()
{
  prepare libpng-1.6.16 libpng-1.6.16.tar.xz || return

  cmakeBuild -D PNG_SHARED=0 \
             -D ZLIB_INCLUDE_DIR="$buildDir/usr/include" \
             -D ZLIB_LIBRARY="$buildDir/usr/lib/libz.a"

  finish
}

function build_jpeglib()
{
  prepare jpeg-9a jpegsrc.v9a.tar.gz || return

  autotoolsBuild --disable-shared

  finish
}

function build_libogg()
{
  prepare libogg-1.3.2 libogg-1.3.2.tar.xz || return
  applyPatches libogg-1.3.2.patch

  autotoolsBuild --disable-shared

  finish
}

function build_libvorbis()
{
  prepare libvorbis-1.3.5 libvorbis-1.3.5.tar.xz || return

  autotoolsBuild --disable-shared --with-ogg="$buildDir/usr"

  finish
}

function build_freetype()
{
  prepare freetype-2.5.5 freetype-2.5.5.tar.bz2 || return

  autotoolsBuild --without-bzip2 --without-png --disable-shared

  finish
}

function build_physfs()
{
  prepare physfs-2.0.3 physfs-2.0.3.tar.bz2 || return

  cmakeBuild -D PHYSFS_BUILD_SHARED=0 -D PHYSFS_BUILD_TEST=0 \
	     -D ZLIB_INCLUDE_DIR="$buildDir/usr/include" -D ZLIB_LIBRARY="$buldDir/usr/lib/zlib.a"

  finish
}

function build_lua()
{
  prepare lua-5.3.1 lua-5.3.1.tar.gz || return
  #applyPatches lua-5.3.0.patch

  if [[ $platform == Emscripten ]]; then
    emmake make -j4 CFLAGS="$CFLAGS" PLAT="generic" MYLIBS="$LDFLAGS"
    emmake make INSTALL_TOP="$buildDir/usr" install
  else
    make -j4 CC="$CC" AR="$AR rcu" RANLIB="$RANLIB" CFLAGS="$CFLAGS" PLAT="generic" MYLIBS="$LDFLAGS"
    make INSTALL_TOP="$buildDir/usr" install
  fi

  finish
}

function build_openal()
{
  prepare openal-soft-1.16.0 openal-soft-1.16.0.tar.bz2 || return

  cmakeBuild -D ALSOFT_UTILS=0 -D ALSOFT_EXAMPLES=0 -D LIBTYPE="STATIC"

  finish
}

function build_sdl2()
{
  prepare SDL2-2.0.3 SDL2-2.0.3.tar.gz || return
  applyPatches SDL2-2.0.3.patch

  cmakeBuild

  finish
}

function build_sdl2_ttf()
{
  prepare SDL2_ttf-2.0.12 SDL2_ttf-2.0.12.tar.gz || return
  applyPatches SDL2_ttf-2.0.12.patch

  autotoolsBuild \
    --with-freetype-prefix="$buildDir/usr" \
    --with-sdl-prefix="$buildDir/usr" \
    --without-x \
    --disable-shared

  finish
}

function build()
{
  # zlib
  setup_emscripten  && build_zlib

  # libpng
  setup_ndk_i686    && build_libpng
  setup_ndk_ARMv7a  && build_libpng

  # jpeglib
  setup_emscripten  && build_jpeglib

  # libogg
  setup_ndk_i686    && build_libogg
  setup_ndk_ARMv7a  && build_libogg
  setup_emscripten  && build_libogg

  # libvorbis
  setup_ndk_i686    && build_libvorbis
  setup_ndk_ARMv7a  && build_libvorbis
  setup_emscripten  && build_libvorbis

  # FreeType
  setup_ndk_i686    && build_freetype
  setup_ndk_ARMv7a  && build_freetype

  # PhysicsFS
  setup_ndk_i686    && build_physfs
  setup_ndk_ARMv7a  && build_physfs
  setup_emscripten  && build_physfs

  # Lua
  setup_ndk_i686    && build_lua
  setup_ndk_ARMv7a  && build_lua
  setup_emscripten  && build_lua

  # OpenAL Soft
  setup_ndk_i686    && build_openal
  setup_ndk_ARMv7a  && build_openal

  # SDL
  setup_ndk_i686    && build_sdl2
  setup_ndk_ARMv7a  && build_sdl2

  # SDL_ttf
  setup_ndk_i686    && build_sdl2_ttf
  setup_ndk_ARMv7a  && build_sdl2_ttf
}

case $1 in
  clean)
    clean
    ;;
  buildclean)
    buildclean
    ;;
  fetch)
    fetch
    ;;
  *)
    build
    ;;
esac
