#!/bin/sh

isBuild=1
. ./configure-all.sh

if (( $Linux_x86_64 )); then
  outDir="support/Linux-x86_64"
  prefix="/usr/lib"

  rm -rf "$outDir"
  mkdir -p "$outDir"

  cp "$prefix/libz.so.1" \
     "$prefix/libphysfs.so.1" \
     "$prefix/liblua.so.5.1" \
     "$prefix/libbz2.so.1.0" \
     "$prefix/libfreetype.so.6" \
     "$prefix/libogg.so.0" \
     "$prefix/libvorbis.so.0" \
     "$prefix/libvorbisfile.so.3" \
     "$prefix/libfreeimage.so.3" \
     "$outDir"

  if [[ -e "$prefix/libmad.so.0" ]]; then
    cp "$prefix/libmad.so.0" \
       "$outDir"
  fi
  if [[ -e "$prefix/libfaad.so.2" ]]; then
    cp "$prefix/libfaad.so.2" \
       "$outDir"
  fi

  if grep SDL2 build/Linux-x86_64/CMakeCache.txt &> /dev/null; then
    cp "$prefix/libSDL2-2.0.so.0" \
       "$prefix/libSDL2_ttf-2.0.so.0" \
       "$outDir"
  else
    cp "$prefix/libSDL-1.2.so.0" \
       "$prefix/libSDL_ttf-2.0.so.0" \
       "$outDir"
  fi
fi

if (( $Linux_x86_32 )); then
  outDir="support/Linux-i686"
  prefix="/usr/lib32"

  rm -rf "$outDir"
  mkdir -p "$outDir"

  cp "$prefix/libz.so.1" \
     "$prefix/libphysfs.so.1" \
     "$prefix/liblua.so.5.1" \
     "$prefix/libbz2.so.1.0" \
     "$prefix/libfreetype.so.6" \
     "$prefix/libogg.so.0" \
     "$prefix/libvorbis.so.0" \
     "$prefix/libvorbisfile.so.3" \
     "$prefix/libfreeimage.so.3" \
     "$outDir"

  if [[ -e "$prefix/libmad.so.0" ]]; then
    cp "$prefix/libmad.so.0" \
       "$outDir"
  fi
  if [[ -e "$prefix/libfaad.so.2" ]]; then
    cp "$prefix/libfaad.so.2" \
       "$outDir"
  fi

  if grep SDL2 build/Linux-x86_32/CMakeCache.txt &> /dev/null; then
    cp "$prefix/libSDL2-2.0.so.0" \
       "$prefix/libSDL2_ttf-2.0.so.0" \
       "$outDir"
  else
    cp "$prefix/libSDL-1.2.so.0" \
       "$prefix/libSDL_ttf-2.0.so.0" \
       "$outDir"
  fi
fi

if (( $Windows_x86_32 )); then
  outDir="support/Windows-i686"
  prefix="/usr/i486-mingw32/bin"

  rm -rf "$outDir"
  mkdir -p "$outDir"

  cp "$prefix/libgcc_s_sjlj-1.dll" \
     "$prefix/libstdc++-6.dll" \
     "$prefix/zlib1.dll" \
     "$prefix/libphysfs.dll" \
     "$prefix/lua52.dll" \
     "$prefix/libfreetype-6.dll" \
     "$prefix/libogg-0.dll" \
     "$prefix/libvorbis-0.dll" \
     "$prefix/libvorbisfile-3.dll" \
     "$prefix/FreeImage.dll" \
     "$outDir"

  if grep SDL2 build/Windows-x86_32/CMakeCache.txt &> /dev/null; then
    cp "$prefix/SDL2.dll" \
       "$prefix/SDL2_ttf.dll" \
       "$outDir"
  else
    cp "$prefix/SDL.dll" \
       "$prefix/SDL_ttf.dll" \
       "$outDir"
  fi
fi
