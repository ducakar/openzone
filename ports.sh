#!/bin/sh

platforms=( \
  NaCl-x86_64 NaCl-i686 \
  Android14-i686 Android14-ARM Android14-ARMv7a Android14-MIPS \
)

projectDir=`pwd`
topDir="$projectDir/ports"

naclPrefix="/home/davorin/Projects/nacl_sdk/pepper_23/toolchain/linux_x86_newlib"

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
  platform="NaCl-x86_64"
  buildDir="$topDir/$platform"
  triplet="x86_64-nacl"
  sysroot="$naclPrefix/x86_64-nacl"
  toolsroot="$naclPrefix"
  toolchain="$projectDir/cmake/$platform.Toolchain.cmake"

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
  platform="NaCl-i686"
  buildDir="$topDir/$platform"
  triplet="i686-nacl"
  sysroot="$naclPrefix/i686-nacl"
  toolsroot="$naclPrefix"
  toolchain="$projectDir/cmake/$platform.Toolchain.cmake"

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

function setup_ndkX86()
{
  platform="Android14-i686"
  buildDir="$topDir/$platform"
  triplet="i686-linux-android"
  sysroot="$ndkX86Platform"
  toolsroot="$ndkX86Tools"
  toolchain="$projectDir/cmake/$platform.Toolchain.cmake"

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
  platform="Android14-ARM"
  buildDir="$topDir/$platform"
  triplet="arm-linux-androideabi"
  sysroot="$ndkARMPlatform"
  toolsroot="$ndkARMTools"
  toolchain="$projectDir/cmake/$platform.Toolchain.cmake"

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
  platform="Android14-ARMv7a"
  buildDir="$topDir/$platform"
  triplet="arm-linux-androideabi"
  sysroot="$ndkARMPlatform"
  toolsroot="$ndkARMTools"
  toolchain="$projectDir/cmake/$platform.Toolchain.cmake"

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
  platform="Android14-MIPS"
  buildDir="$topDir/$platform"
  triplet="mipsel-linux-android"
  sysroot="$ndkMIPSPlatform"
  toolsroot="$ndkMIPSTools"
  toolchain="$projectDir/cmake/$platform.Toolchain.cmake"

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

function download()
{
  mkdir -p "$topDir/archives"
  wget -c -nc -P "$topDir/archives" $@
}

function fetch()
{
  # zlib
  download 'http://zlib.net/zlib-1.2.7.tar.bz2'

  # physfs
  download 'http://icculus.org/physfs/downloads/physfs-2.0.2.tar.gz'

  # physfs 2.1
  cd "$topDir/archives"
  if [[ -d "physfs" ]]
    then cd "physfs" && hg pull -u
    else hg clone 'http://hg.icculus.org/icculus/physfs/'
  fi

  # lua
  download 'http://www.lua.org/ftp/lua-5.2.1.tar.gz'

  # SDL
  download 'http://www.libsdl.org/release/SDL-1.2.15.tar.gz'

  # SDL2
  cd "$topDir/archives"
  if [[ -d "SDL" ]]
    then cd "SDL" && hg pull -u
    else hg clone 'http://hg.libsdl.org/SDL'
  fi

  # SDL_ttf
  download 'http://www.libsdl.org/projects/SDL_ttf/release/SDL_ttf-2.0.11.tar.gz'

  # SDL2_ttf
  cd "$topDir/archives"
  if [[ -d "SDL_ttf" ]]
    then cd "SDL_ttf" && hg pull -u
    else hg clone 'http://hg.libsdl.org/SDL_ttf'
  fi

  # freetype
  download 'http://sourceforge.net/projects/freetype/files/freetype2/2.4.10/freetype-2.4.10.tar.bz2'

  # openal
  download 'http://kcat.strangesoft.net/openal-releases/openal-soft-1.14.tar.bz2'

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
  tar xf "$topDir/archives/$2" -C "$buildDir"

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
  ../configure --host=$triplet --prefix=/usr $@ || exit 1

  make -j4 || exit 1
  make install DESTDIR="$buildDir"
}

function build_zlib()
{
  prepare "zlib-1.2.7" "zlib-1.2.7.tar.bz2" || return

  export CFLAGS="$CPPFLAGS $CFLAGS"
  ./configure --prefix=/usr --static

  make -j4 || return 1
  make install DESTDIR="$buildDir"

  rm -rf "$buildDir"/usr/lib/libz.so*
}

function build_physfs()
{
  prepare "physfs-2.0.2" "physfs-2.0.2.tar.gz" || return
  applyPatches "physfs-2.0.2.patch"

  cmakeBuild -D PHYSFS_BUILD_SHARED=0 -D PHYSFS_BUILD_TEST=0
}

function build_physfs21()
{
  [[ -d "$buildDir/physfs" ]] && return

  msg "physfs-2.1-hg @ $platform"

  cp -R "$topDir/archives/physfs" "$buildDir"
  cd "$buildDir/physfs"
  applyPatches "physfs-2.1.patch"

  cmakeBuild -D PHYSFS_BUILD_SHARED=0 -D PHYSFS_BUILD_TEST=0
}

function build_lua()
{
  prepare "lua-5.2.1" "lua-5.2.1.tar.gz" || return
  applyPatches "lua-5.2.1.patch"

  make -j4 CC="$CC" CFLAGS="$CPPFLAGS $CFLAGS" PLAT="generic" MYLIBS="$LDFLAGS"
  make INSTALL_TOP="$buildDir/usr" install
}

function build_sdl()
{
  prepare "SDL-1.2.15" "SDL-1.2.15.tar.gz" || return
  applyPatches "SDL-1.2.15.patch"

  ./autogen.sh
  case $triplet in
    *-nacl)
      # assembly causes validity check to fail
      autotoolsBuild --disable-shared --disable-pthread-sem --disable-assembly
      ;;
    *)
      autotoolsBuild --disable-shared
      ;;
  esac
}

function build_sdl2()
{
  [[ -d "$buildDir/SDL" ]] && return

  msg "SDL-2.0-hg @ $platform"

  cp -R "$topDir/archives/SDL" "$buildDir"
  cp "$projectDir/etc/SDL2-CMakeLists-gen.sh" "$buildDir/SDL"
  cd "$buildDir/SDL"
  applyPatches "SDL-hg.patch"

  ./SDL2-CMakeLists-gen.sh
  cmakeBuild
}

function build_freetype()
{
  prepare "freetype-2.4.10" "freetype-2.4.10.tar.bz2" || return

  autotoolsBuild --disable-shared --without-bzip2
}

function build_sdl_ttf()
{
  prepare "SDL_ttf-2.0.11" "SDL_ttf-2.0.11.tar.gz" || return
  applyPatches "SDL_ttf-2.0.11.patch"

  autotoolsBuild \
    --with-freetype-prefix="$buildDir/usr" \
    --with-sdl-prefix="$buildDir/usr" \
    --without-x
}

function build_sdl2_ttf()
{
  [[ -d "$buildDir/SDL_ttf" ]] && return

  msg "SDL2_ttf @ $platform"

  cp -R "$topDir/archives/SDL_ttf" "$buildDir"
  cd "$buildDir/SDL_ttf"
  applyPatches "SDL_ttf-hg.patch"

  export CFLAGS="$CFLAGS -I$buildDir/usr/include/SDL2"
  autotoolsBuild \
    --with-freetype-prefix="$buildDir/usr" \
    --with-sdl-prefix="$buildDir/usr" \
    --without-x
}

function build_openal()
{
  prepare "openal-soft-1.14" "openal-soft-1.14.tar.bz2" || return
  applyPatches "openal-soft-1.14.patch"

  cmakeBuild -D UTILS=0 -D EXAMPLES=0 -D LIBTYPE=STATIC
}

function build_libogg()
{
  prepare "libogg-1.3.0" "libogg-1.3.0.tar.xz" || return
  applyPatches "libogg-1.3.0.patch"

  autotoolsBuild
}

function build_libvorbis()
{
  prepare "libvorbis-1.3.3" "libvorbis-1.3.3.tar.xz" || return
  applyPatches "libvorbis-1.3.3.patch"

  autotoolsBuild
}

function build()
{
  # zlib
  setup_nacl64  && CFLAGS="$CFLAGS -Dunlink=puts" build_zlib
  setup_nacl32  && CFLAGS="$CFLAGS -Dunlink=puts" build_zlib
  setup_ndkX86  && build_zlib
  setup_ndkARM  && build_zlib
  setup_ndkARM7 && build_zlib
  setup_ndkMIPS && build_zlib

  # physfs
  setup_nacl64  && build_physfs21
  setup_nacl32  && build_physfs21
  setup_ndkX86  && build_physfs21
  setup_ndkARM  && build_physfs21
  setup_ndkARM7 && build_physfs21
  setup_ndkMIPS && build_physfs21

  # lua
  setup_nacl64  && build_lua
  setup_nacl32  && build_lua
  setup_ndkX86  && build_lua
  setup_ndkARM  && build_lua
  setup_ndkARM7 && build_lua
  setup_ndkMIPS && build_lua

  # SDL
  setup_nacl64  && build_sdl
  setup_nacl32  && build_sdl
  setup_ndkX86  && build_sdl2
  setup_ndkARM  && build_sdl2
  setup_ndkARM7 && build_sdl2
  setup_ndkMIPS && build_sdl2

  # freetype
  setup_nacl64  && build_freetype
  setup_nacl32  && build_freetype
  setup_ndkX86  && build_freetype
  setup_ndkARM  && build_freetype
  setup_ndkARM7 && build_freetype
  setup_ndkMIPS && build_freetype

  # SDL_ttf
  setup_nacl64  && build_sdl_ttf
  setup_nacl32  && build_sdl_ttf
  setup_ndkX86  && build_sdl2_ttf
  setup_ndkARM  && build_sdl2_ttf
  setup_ndkARM7 && build_sdl2_ttf
  setup_ndkMIPS && build_sdl2_ttf

  # openal
  setup_nacl64  && build_openal
  setup_nacl32  && build_openal
  setup_ndkX86  && build_openal
  setup_ndkARM  && build_openal
  setup_ndkARM7 && build_openal
  setup_ndkMIPS && build_openal

  # libogg
  setup_nacl64  && build_libogg
  setup_nacl32  && build_libogg
  setup_ndkX86  && build_libogg
  setup_ndkARM  && build_libogg
  setup_ndkARM7 && build_libogg
  setup_ndkMIPS && build_libogg

  # libvorbis
  setup_nacl64  && build_libvorbis
  setup_nacl32  && build_libvorbis
  setup_ndkX86  && build_libvorbis
  setup_ndkARM  && build_libvorbis
  setup_ndkARM7 && build_libvorbis
  setup_ndkMIPS && build_libvorbis
}

case "$1" in
  clean)
    clean
    ;;
  fetch)
    fetch
    ;;
  build|*)
    build
    ;;
esac
