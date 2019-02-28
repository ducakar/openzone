#!/bin/bash
#
# lib.sh [clean]
#
# Copy all libraries OpenZone depends on to `lib/<platform>` directories (currently Linux-x86_64 and
# Windows-x86_64). Those are required to create standalone build (see `OZ_BUNDLE` cmake option) that
# can be distributed in a ZIP archive (i.e. no installation required, all dependencies included).
#
# This script is currently Arch Linux-specific and assumes one has both Linux and MinGW versions of
# all necessary libraries installed. Many of those packages must be built from AUR.
#
# The following commands may be given (`build` is assumed if none):
#
# - `clean`: Delete directories for all platforms.
# - (none): Copy libraries for selected platforms into corresponding directories.

platforms=(
  Linux-x86_64
  Windows-x86_64
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
    if [[ $platform == Linux-x86_64 ]]; then
      outDir=lib/$platform
      prefix=/usr/lib64

      rm -rf $outDir
      mkdir -p $outDir

      cp "$prefix/libphysfs.so.1" \
         "$prefix/libSDL2-2.0.so.0" \
         "$prefix/libSDL2_ttf-2.0.so.0" \
         "$prefix/libopenal.so.1" \
         "$prefix/liblua.so.5.3" \
         "$prefix/libpng16.so.16" \
         "$outDir"

      chmod +x $outDir/*
      strip $outDir/*
    fi

    if [[ $platform == Windows-x86_64 ]]; then
      outDir=lib/$platform
      prefix=/usr/x86_64-w64-mingw32/bin

      rm -rf $outDir
      mkdir -p $outDir

      cp "$prefix/libgcc_s_seh-1.dll" \
         "$prefix/libstdc++-6.dll" \
         "$prefix/libwinpthread-1.dll" \
         "$prefix/zlib1.dll" \
         "$prefix/libbz2-1.dll" \
         "$prefix/libphysfs.dll" \
         "$prefix/SDL2.dll" \
         "$prefix/SDL2_ttf.dll" \
         "$prefix/OpenAL32.dll" \
         "$prefix/libpng16-16.dll" \
         "$prefix/lua52.dll" \
         "$prefix/libfreetype-6.dll" \
         "$prefix/libogg-0.dll" \
         "$prefix/libvorbis-0.dll" \
         "$prefix/libvorbisfile-3.dll" \
         "$outDir"

      chmod +x $outDir/*
      x86_64-w64-mingw32-strip $outDir/*
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
