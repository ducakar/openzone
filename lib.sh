#!/bin/sh
#
# lib.sh [clean]
#
# Copy all libraries OpenZone depends on to `lib/<platform>` directories (currently Linux-x86_64,
# Linux-i686 and Windows-i686). Those are required to create standalone build (see OZ_STANDALONE
# cmake option) that can be distributed in a ZIP archive (i.e. no installation required, all
# dependencies included).
#
# This script is currently Arch Linux-specific and assumes one has all 64-bit, 32-bit (`lib32-*`)
# and MinGW (`mingw32-*`) versions of all necessary libraries installed. Many of those packages must
# be built from AUR.
#
# The following commands may be given (`build` is assumed if none):
#
# - `clean`: Delete directories for all platforms.
# - (none): Copy libraries for selected platforms into corresponding directories.


platforms=(
  Linux-x86_64
  Linux-i686
  Windows-i686
)

function clean()
{
  for platform in ${platforms[@]}; do
    rm -rf lib/$platform
  done
}

function build()
{
  for platform in ${platforms[@]}; do
    if [[ $platform == Linux-x86_64 || $platform == Linux-i686 ]]; then
      outDir=lib/$platform
      prefix=/usr/lib

      [[ $platform == Linux-x86_64 && ! -d /usr/lib32 ]] && prefix=/usr/lib64
      [[ $platform == Linux-i686   &&   -d /usr/lib32 ]] && prefix=/usr/lib32

      rm -rf $outDir
      mkdir -p $outDir

      cp "$prefix/libphysfs.so.1" \
         "$prefix/libSDL-1.2.so.0" \
         "$prefix/libSDL_ttf-2.0.so.0" \
         "$prefix/liblua.so.5.2" \
         "$prefix/libpng16.so.16" \
         "$prefix/libfreeimage.so.3" \
         "$prefix/libnoise.so.1.0.0" \
         "$prefix/libassimp.so.3" \
         "$outDir"

      chmod +x $outDir/*
      strip $outDir/*
    fi

    if [[ $platform == Windows-i686 ]]; then
      outDir=lib/$platform
      prefix=/usr/i486-mingw32/bin

      rm -rf $outDir
      mkdir -p $outDir

      cp "$prefix/libgcc_s_sjlj-1.dll" \
         "$prefix/libstdc++-6.dll" \
         "$prefix/zlib1.dll" \
         "$prefix/libphysfs.dll" \
         "$prefix/SDL.dll" \
         "$prefix/SDL_ttf.dll" \
         "$prefix/libpng16-16.dll" \
         "$prefix/lua52.dll" \
         "$prefix/libfreetype-6.dll" \
         "$prefix/libogg-0.dll" \
         "$prefix/libvorbis-0.dll" \
         "$prefix/libvorbisfile-3.dll" \
         "$prefix/FreeImage.dll" \
         "$prefix/libnoise.dll" \
         "$prefix/libassimp.dll" \
         "$outDir"

      chmod +x $outDir/*
      i486-mingw32-strip $outDir/*
    fi
  done
}

case $1 in
  clean)
    clean
    ;;
  *)
    build
    ;;
esac
