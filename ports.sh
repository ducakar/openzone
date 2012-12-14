#!/bin/sh
#
# ports.sh [clean | fetch | build]
#
# This script is used to build libraries required by OpenZone for some platforms. Currently it
# builds zlib, physfs, SDL, SDL_ttf, OpenAL Soft, squish for NaCl-x86_64 and NaCl-i686.
# The following commands may be given (`build` is assumed if none):
#
# - `clean`: Delete everything in `ports` directory except downloaded sources.
# - `buildclean`: Delete build directories. Downloaded sources and installed libraries are left
#   intact.
# - `fetch`: Download sources into `ports/archives` directory.
# - `build`: Builds all libraries for all platforms.
#

platforms=(
  NaCl-x86_64 NaCl-x86_64-glibc NaCl-i686 NaCl-i686-glibc PNaCl
  Android14-i686 Android14-ARM Android14-ARMv7a Android14-MIPS
)

projectDir=`pwd`
topDir="$projectDir/ports"

# Extract path to NaCl SDK from CMake toolchain files.
naclPrefix=`sed -r '/ PLATFORM_PREFIX / !d; s|.*\"(.*)\".*|\1|' cmake/NaCl-x86_64.Toolchain.cmake`
naclGNUPrefix=`sed -r '/ PLATFORM_PREFIX / !d; s|.*\"(.*)\".*|\1|' cmake/NaCl-x86_64-glibc.Toolchain.cmake`
pnaclPrefix=`sed -r '/ PLATFORM_PREFIX / !d; s|.*\"(.*)\".*|\1|' cmake/PNaCl.Toolchain.cmake`

ndkX86Tools="/opt/android-ndk/toolchains/x86-4.6/prebuilt/linux-x86"
ndkX86Platform="/opt/android-ndk/platforms/android-14/arch-x86"

ndkARMTools="/opt/android-ndk/toolchains/arm-linux-androideabi-4.6/prebuilt/linux-x86"
ndkARMPlatform="/opt/android-ndk/platforms/android-14/arch-arm"

ndkMIPSTools="/opt/android-ndk/toolchains/mipsel-linux-android-4.6/prebuilt/linux-x86"
ndkMIPSPlatform="/opt/android-ndk/platforms/android-14/arch-mips"

function msg()
{
  echo -ne "\e[1;32m"
  echo ================================================================================
  echo
  echo "          $@"
  echo
  echo ================================================================================
  echo -ne "\e[0m"
}


function setup_nacl64()
{
  platform="NaCl-x86_64"                                  # Platform name.
  buildDir="$topDir/$platform"                            # Build and install directory.
  triplet="x86_64-nacl"                                   # Platform triplet (tools prefix).
  hostTriplet="$triplet"                                  # Host triplet for autotools configure.
  sysroot="$naclPrefix/x86_64-nacl"                       # SDK sysroot.
  toolsroot="$naclPrefix"                                 # SDK tool root.
  toolchain="$projectDir/cmake/$platform.Toolchain.cmake" # CMake toolchain.

  export CPP="$toolsroot/bin/$triplet-cpp"
  export CC="$toolsroot/bin/$triplet-gcc"
  export AR="$toolsroot/bin/$triplet-ar"
  export RANLIB="$toolsroot/bin/$triplet-ranlib"
  export STRIP="$toolsroot/bin/$triplet-strip"
  export PKG_CONFIG_PATH="$buildDir/usr/lib/pkgconfig"
  export PKG_CONFIG_LIBDIR="$buildDir/usr/lib"
  export PATH="$toolsroot/bin:$PATH"

  export CPPFLAGS="-I$buildDir/usr/include"
  export CFLAGS="-O3 -ffast-math -msse3"
  export LDFLAGS="-L$buildDir/usr/lib"
}

function setup_nacl64GNU()
{
  platform="NaCl-x86_64-glibc"                            # Platform name.
  buildDir="$topDir/$platform"                            # Build and install directory.
  triplet="x86_64-nacl"                                   # Platform triplet (tools prefix).
  hostTriplet="$triplet"                                  # Host triplet for autotools configure.
  sysroot="$naclGNUPrefix/x86_64-nacl"                    # SDK sysroot.
  toolsroot="$naclGNUPrefix"                              # SDK tool root.
  toolchain="$projectDir/cmake/$platform.Toolchain.cmake" # CMake toolchain.

  export CPP="$toolsroot/bin/$triplet-cpp"
  export CC="$toolsroot/bin/$triplet-gcc"
  export AR="$toolsroot/bin/$triplet-ar"
  export RANLIB="$toolsroot/bin/$triplet-ranlib"
  export STRIP="$toolsroot/bin/$triplet-strip"
  export PKG_CONFIG_PATH="$buildDir/usr/lib/pkgconfig"
  export PKG_CONFIG_LIBDIR="$buildDir/usr/lib"
  export PATH="$toolsroot/bin:$PATH"

  export CPPFLAGS="-I$buildDir/usr/include"
  export CFLAGS="-O3 -ffast-math -msse3"
  export LDFLAGS="-L$buildDir/usr/lib -lnosys"
}

function setup_nacl32()
{
  platform="NaCl-i686"                                    # Platform name.
  buildDir="$topDir/$platform"                            # Build and install directory.
  triplet="i686-nacl"                                     # Platform triplet (tools prefix).
  hostTriplet="$triplet"                                  # Host triplet for autotools configure.
  sysroot="$naclPrefix/i686-nacl"                         # SDK sysroot.
  toolsroot="$naclPrefix"                                 # SDK tool root.
  toolchain="$projectDir/cmake/$platform.Toolchain.cmake" # CMake toolchain.

  export CPP="$toolsroot/bin/$triplet-cpp"
  export CC="$toolsroot/bin/$triplet-gcc"
  export AR="$toolsroot/bin/$triplet-ar"
  export RANLIB="$toolsroot/bin/$triplet-ranlib"
  export STRIP="$toolsroot/bin/$triplet-strip"
  export PKG_CONFIG_PATH="$buildDir/usr/lib/pkgconfig"
  export PKG_CONFIG_LIBDIR="$buildDir/usr/lib"
  export PATH="$toolsroot/bin:$PATH"

  export CPPFLAGS="-I$buildDir/usr/include"
  export CFLAGS="-O3 -ffast-math -msse3 -mfpmath=sse"
  export LDFLAGS="-L$buildDir/usr/lib -lnosys"
}

function setup_nacl32GNU()
{
  platform="NaCl-i686-glibc"                              # Platform name.
  buildDir="$topDir/$platform"                            # Build and install directory.
  triplet="i686-nacl"                                     # Platform triplet (tools prefix).
  hostTriplet="$triplet"                                  # Host triplet for autotools configure.
  sysroot="$naclGNUPrefix/i686-nacl"                      # SDK sysroot.
  toolsroot="$naclGNUPrefix"                              # SDK tool root.
  toolchain="$projectDir/cmake/$platform.Toolchain.cmake" # CMake toolchain.

  export CPP="$toolsroot/bin/$triplet-cpp"
  export CC="$toolsroot/bin/$triplet-gcc"
  export AR="$toolsroot/bin/$triplet-ar"
  export RANLIB="$toolsroot/bin/$triplet-ranlib"
  export STRIP="$toolsroot/bin/$triplet-strip"
  export PKG_CONFIG_PATH="$buildDir/usr/lib/pkgconfig"
  export PKG_CONFIG_LIBDIR="$buildDir/usr/lib"
  export PATH="$toolsroot/bin:$PATH"

  export CPPFLAGS="-I$buildDir/usr/include"
  export CFLAGS="-O3 -ffast-math -msse3 -mfpmath=sse"
  export LDFLAGS="-L$buildDir/usr/lib"
}

function setup_pnacl()
{
  platform="PNaCl"                                        # Platform name.
  buildDir="$topDir/$platform"                            # Build and install directory.
  triplet="pnacl"                                         # Platform triplet (tools prefix).
  hostTriplet="i686-nacl"                                 # Host triplet for autotools configure.
  sysroot="$pnaclPrefix/usr"                              # SDK sysroot.
  toolsroot="$pnaclPrefix"                                # SDK tool root.
  toolchain="$projectDir/cmake/$platform.Toolchain.cmake" # CMake toolchain.

  #export CPP="$toolsroot/bin/$triplet-cpp"
  export -n CPP
  export CC="$toolsroot/bin/$triplet-clang"
  export AR="$toolsroot/bin/$triplet-ar"
  export RANLIB="$toolsroot/bin/$triplet-ranlib"
  export STRIP="$toolsroot/bin/$triplet-strip"
  export PKG_CONFIG_PATH="$buildDir/usr/lib/pkgconfig"
  export PKG_CONFIG_LIBDIR="$buildDir/usr/lib"
  export PATH="$toolsroot/bin:$PATH"

  export CPPFLAGS="-I$buildDir/usr/include"
  export CFLAGS="-O3 -ffast-math"
  export LDFLAGS="-L$buildDir/usr/lib -lnosys"
}

function setup_ndkX86()
{
  platform="Android14-i686"                               # Platform name.
  buildDir="$topDir/$platform"                            # Build and install directory.
  triplet="i686-linux-android"                            # Platform triplet (tools prefix).
  hostTriplet="$triplet"                                  # Host triplet for autotools configure.
  sysroot="$ndkX86Platform"                               # SDK sysroot.
  toolsroot="$ndkX86Tools"                                # SDK tool root.
  toolchain="$projectDir/cmake/$platform.Toolchain.cmake" # CMake toolchain.

  export CPP="$toolsroot/bin/$triplet-cpp"
  export CC="$toolsroot/bin/$triplet-gcc"
  export AR="$toolsroot/bin/$triplet-ar"
  export RANLIB="$toolsroot/bin/$triplet-ranlib"
  export STRIP="$toolsroot/bin/$triplet-strip"
  export PKG_CONFIG_PATH="$buildDir/usr/lib/pkgconfig"
  export PKG_CONFIG_LIBDIR="$buildDir/usr/lib"
  export PATH="$toolsroot/bin:$PATH"

  export CPPFLAGS="--sysroot=$sysroot -I$buildDir/usr/include"
  export CFLAGS="-Ofast -fPIC -msse3 -mfpmath=sse"
  export LDFLAGS="--sysroot=$sysroot -L$buildDir/usr/lib"
}

function setup_ndkARM()
{
  platform="Android14-ARM"                                # Platform name.
  buildDir="$topDir/$platform"                            # Build and install directory.
  triplet="arm-linux-androideabi"                         # Platform triplet (tools prefix).
  hostTriplet="$triplet"                                  # Host triplet for autotools configure.
  sysroot="$ndkARMPlatform"                               # SDK sysroot.
  toolsroot="$ndkARMTools"                                # SDK tool root.
  toolchain="$projectDir/cmake/$platform.Toolchain.cmake" # CMake toolchain.

  export CPP="$toolsroot/bin/$triplet-cpp"
  export CC="$toolsroot/bin/$triplet-gcc"
  export AR="$toolsroot/bin/$triplet-ar"
  export RANLIB="$toolsroot/bin/$triplet-ranlib"
  export STRIP="$toolsroot/bin/$triplet-strip"
  export PKG_CONFIG_PATH="$buildDir/usr/lib/pkgconfig"
  export PKG_CONFIG_LIBDIR="$buildDir/usr/lib"
  export PATH="$toolsroot/bin:$PATH"

  export CPPFLAGS="--sysroot=$sysroot -I$buildDir/usr/include"
  export CFLAGS="-Wno-psabi -Ofast -fPIC"
  export LDFLAGS="--sysroot=$sysroot -L$buildDir/usr/lib"
}

function setup_ndkARM7()
{
  platform="Android14-ARMv7a"                             # Platform name.
  buildDir="$topDir/$platform"                            # Build and install directory.
  triplet="arm-linux-androideabi"                         # Platform triplet (tools prefix).
  hostTriplet="$triplet"                                  # Host triplet for autotools configure.
  sysroot="$ndkARMPlatform"                               # SDK sysroot.
  toolsroot="$ndkARMTools"                                # SDK tool root.
  toolchain="$projectDir/cmake/$platform.Toolchain.cmake" # CMake toolchain.

  export CPP="$toolsroot/bin/$triplet-cpp"
  export CC="$toolsroot/bin/$triplet-gcc"
  export AR="$toolsroot/bin/$triplet-ar"
  export RANLIB="$toolsroot/bin/$triplet-ranlib"
  export STRIP="$toolsroot/bin/$triplet-strip"
  export PKG_CONFIG_PATH="$buildDir/usr/lib/pkgconfig"
  export PKG_CONFIG_LIBDIR="$buildDir/usr/lib"
  export PATH="$toolsroot/bin:$PATH"

  export CPPFLAGS="--sysroot=$sysroot -I$buildDir/usr/include"
  export CFLAGS="-Wno-psabi -Ofast -fPIC -march=armv7-a -mfloat-abi=softfp -mfpu=neon"
  export LDFLAGS="--sysroot=$sysroot -L$buildDir/usr/lib -Wl,--fix-cortex-a8"
}

function setup_ndkMIPS()
{
  platform="Android14-MIPS"                               # Platform name.
  buildDir="$topDir/$platform"                            # Build and install directory.
  triplet="mipsel-linux-android"                          # Platform triplet (tools prefix).
  hostTriplet="$triplet"                                  # Host triplet for autotools configure.
  sysroot="$ndkMIPSPlatform"                              # SDK sysroot.
  toolsroot="$ndkMIPSTools"                               # SDK tool root.
  toolchain="$projectDir/cmake/$platform.Toolchain.cmake" # CMake toolchain.

  export CPP="$toolsroot/bin/$triplet-cpp"
  export CC="$toolsroot/bin/$triplet-gcc"
  export AR="$toolsroot/bin/$triplet-ar"
  export RANLIB="$toolsroot/bin/$triplet-ranlib"
  export STRIP="$toolsroot/bin/$triplet-strip"
  export PKG_CONFIG_PATH="$buildDir/usr/lib/pkgconfig"
  export PKG_CONFIG_LIBDIR="$buildDir/usr/lib"
  export PATH="$toolsroot/bin:$PATH"

  export CPPFLAGS="--sysroot=$sysroot -I$buildDir/usr/include"
  export CFLAGS="-Ofast -fPIC"
  export LDFLAGS="--sysroot=$sysroot -L$buildDir/usr/lib"
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
  download 'http://zlib.net/zlib-1.2.7.tar.bz2'

  # physfs 2.1
  cd "$topDir/archives"
  if [[ -d physfs ]]
    then cd physfs && hg pull -u
    else hg clone 'http://hg.icculus.org/icculus/physfs/'
  fi

  # lua
  download 'http://www.lua.org/ftp/lua-5.2.1.tar.gz'

  # SDL
  download 'http://www.libsdl.org/release/SDL-1.2.15.tar.gz'

  # SDL2
  cd "$topDir/archives"
  if [[ -d SDL ]]
    then cd SDL && hg pull -u
    else hg clone 'http://hg.libsdl.org/SDL'
  fi

  # SDL_ttf
  download 'http://www.libsdl.org/projects/SDL_ttf/release/SDL_ttf-2.0.11.tar.gz'

  # SDL2_ttf
  cd "$topDir/archives"
  if [[ -d SDL_ttf ]]
    then cd SDL_ttf && hg pull -u
    else hg clone 'http://hg.libsdl.org/SDL_ttf'
  fi

  # freetype
  download 'http://sourceforge.net/projects/freetype/files/freetype2/2.4.10/freetype-2.4.10.tar.bz2'

  # openal
  download 'http://kcat.strangesoft.net/openal-releases/openal-soft-1.15.1.tar.bz2'

  # libogg
  download 'http://downloads.xiph.org/releases/ogg/libogg-1.3.0.tar.xz'

  # libvorbis
  download 'http://downloads.xiph.org/releases/vorbis/libvorbis-1.3.3.tar.xz'
}

function prepare()
{
  [[ -d "$buildDir/$1" ]] && return 1

  msg "$1 @ $platform"

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

  cmake \
    -D CMAKE_TOOLCHAIN_FILE="$toolchain" \
    -D CMAKE_BUILD_TYPE="Release" \
    -D CMAKE_INSTALL_PREFIX="/usr" \
    -D PLATFORM_PORTS_PREFIX="$buildDir" \
    $@ \
    .. || return 1

  make -j4 || exit 1
  make install DESTDIR="$buildDir"
}

function autotoolsBuild()
{
  mkdir -p build && cd build
  ../configure --host=$hostTriplet --prefix=/usr $@ || exit 1

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
  prepare zlib-1.2.7 zlib-1.2.7.tar.bz2 || return

  CFLAGS="$CPPFLAGS $CFLAGS -Dunlink=puts" ./configure --prefix=/usr --static
  make -j4 || return 1
  make install DESTDIR="$buildDir"
  rm -rf "$buildDir"/usr/lib/libz.so*

  finish
}

function build_physfs()
{
  prepare physfs physfs || return
  applyPatches physfs-2.1.patch

  cmakeBuild -D PHYSFS_BUILD_SHARED=0 -D PHYSFS_BUILD_TEST=0

  finish
}

function build_lua()
{
  prepare lua-5.2.1 lua-5.2.1.tar.gz || return
  applyPatches lua-5.2.1.patch

  make -j4 CC="$CC" AR="$AR rcu" RANLIB="$RANLIB" CFLAGS="$CFLAGS" PLAT="generic" MYLIBS="$LDFLAGS"
  make INSTALL_TOP="$buildDir/usr" install

  finish
}

function build_sdl()
{
  prepare SDL-1.2.15 SDL-1.2.15.tar.gz || return
  applyPatches SDL-1.2.15.patch

  ./autogen.sh
  case $triplet in
    *nacl)
      # Assembly causes NaCl validity check to fail when NEXE is loading.
      autotoolsBuild --disable-shared --disable-pthread-sem --disable-assembly
      ;;
    *)
      autotoolsBuild --disable-shared
      ;;
  esac

  finish
}

function build_sdl2()
{
  prepare SDL SDL || return
  applyPatches SDL-hg.patch

  cp "$projectDir/etc/SDL2-CMakeLists-gen.sh" "$buildDir/SDL"
  buildDir="$buildDir" ./SDL2-CMakeLists-gen.sh
  cmakeBuild

  finish
}

function build_freetype()
{
  prepare freetype-2.4.10 freetype-2.4.10.tar.bz2 || return

  autotoolsBuild --disable-shared --without-bzip2

  finish
}

function build_sdl_ttf()
{
  prepare SDL_ttf-2.0.11 SDL_ttf-2.0.11.tar.gz || return
  applyPatches SDL_ttf-2.0.11.patch

  autotoolsBuild \
    --with-freetype-prefix="$buildDir/usr" \
    --with-sdl-prefix="$buildDir/usr" \
    --without-x

  finish
}

function build_sdl2_ttf()
{
  prepare SDL_ttf SDL_ttf || return
  applyPatches SDL_ttf-hg.patch

  autotoolsBuild \
    --with-freetype-prefix="$buildDir/usr" \
    --with-sdl-prefix="$buildDir/usr" \
    --without-x

  finish
}

function build_openal()
{
  prepare openal-soft-1.15.1 openal-soft-1.15.1.tar.bz2 || return
  applyPatches openal-soft-1.15.1.patch

  cmakeBuild -D UTILS=0 -D EXAMPLES=0 -D LIBTYPE=STATIC

  finish
}

function build_libogg()
{
  prepare libogg-1.3.0 libogg-1.3.0.tar.xz || return
  applyPatches libogg-1.3.0.patch

  autotoolsBuild

  finish
}

function build_libvorbis()
{
  prepare libvorbis-1.3.3 libvorbis-1.3.3.tar.xz || return
  applyPatches libvorbis-1.3.3.patch

  autotoolsBuild

  finish
}

function build()
{
  # zlib
  setup_nacl64    && build_zlib
  setup_nacl64GNU && build_zlib
  setup_nacl32    && build_zlib
  setup_nacl32GNU && build_zlib
  setup_pnacl     && build_zlib

  # physfs
  setup_nacl64    && build_physfs
  setup_nacl64GNU && build_physfs
  setup_nacl32    && build_physfs
  setup_nacl32GNU && build_physfs
  setup_pnacl     && build_physfs
  setup_ndkX86    && build_physfs
  setup_ndkARM    && build_physfs
  setup_ndkARM7   && build_physfs
  setup_ndkMIPS   && build_physfs

  # lua
  setup_nacl64    && build_lua
  setup_nacl64GNU && build_lua
  setup_nacl32    && build_lua
  setup_nacl32GNU && build_lua
  setup_pnacl     && build_lua
  setup_ndkX86    && build_lua
  setup_ndkARM    && build_lua
  setup_ndkARM7   && build_lua
  setup_ndkMIPS   && build_lua

  # SDL
  setup_nacl64    && build_sdl
  setup_nacl64GNU && build_sdl
  setup_nacl32    && build_sdl
  setup_nacl32GNU && build_sdl
  setup_pnacl     && build_sdl
  setup_ndkX86    && build_sdl2
  setup_ndkARM    && build_sdl2
  setup_ndkARM7   && build_sdl2
  setup_ndkMIPS   && build_sdl2

  # freetype
  setup_nacl64    && build_freetype
  setup_nacl64GNU && build_freetype
  setup_nacl32    && build_freetype
  setup_nacl32GNU && build_freetype
  setup_pnacl     && build_freetype
  setup_ndkX86    && build_freetype
  setup_ndkARM    && build_freetype
  setup_ndkARM7   && build_freetype
  setup_ndkMIPS   && build_freetype

  # SDL_ttf
  setup_nacl64    && build_sdl_ttf
  setup_nacl64GNU && build_sdl_ttf
  setup_nacl32    && build_sdl_ttf
  setup_nacl32GNU && build_sdl_ttf
  setup_pnacl     && build_sdl_ttf
  setup_ndkX86    && build_sdl2_ttf
  setup_ndkARM    && build_sdl2_ttf
  setup_ndkARM7   && build_sdl2_ttf
  setup_ndkMIPS   && build_sdl2_ttf

  # openal
  setup_nacl64    && build_openal
  setup_nacl64GNU && build_openal
  setup_nacl32    && build_openal
  setup_nacl32GNU && build_openal
  setup_pnacl     && build_openal
  setup_ndkX86    && build_openal
  setup_ndkARM    && build_openal
  setup_ndkARM7   && build_openal
  setup_ndkMIPS   && build_openal

  # libogg
  setup_nacl64    && build_libogg
  setup_nacl64GNU && build_libogg
  setup_nacl32    && build_libogg
  setup_nacl32GNU && build_libogg
  setup_pnacl     && build_libogg
  setup_ndkX86    && build_libogg
  setup_ndkARM    && build_libogg
  setup_ndkARM7   && build_libogg
  setup_ndkMIPS   && build_libogg

  # libvorbis
  setup_nacl64    && build_libvorbis
  setup_nacl64GNU && build_libvorbis
  setup_nacl32    && build_libvorbis
  setup_nacl32GNU && build_libvorbis
  setup_pnacl     && build_libvorbis
  setup_ndkX86    && build_libvorbis
  setup_ndkARM    && build_libvorbis
  setup_ndkARM7   && build_libvorbis
  setup_ndkMIPS   && build_libvorbis
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
  build|*)
    build
    ;;
esac
