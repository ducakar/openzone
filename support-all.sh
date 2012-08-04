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
     "$prefix/libSDL-1.2.so.0" \
     "$prefix/libSDL_ttf-2.0.so.0" \
     "$prefix/liblua.so.5.1" \
     "$prefix/libbz2.so.1.0" \
     "$prefix/libfreetype.so.6" \
     "$prefix/libogg.so.0" \
     "$prefix/libvorbis.so.0" \
     "$prefix/libvorbisfile.so.3" \
     "$prefix/libfreeimage.so.3" \
     "$outDir"

  cp "build/Linux-x86_64/src/txc_dxtn/libtxc_dxtn.so" \
     "$outDir"

  chmod +x "$outDir/"*
fi

if (( $Linux_i686 )); then
  outDir="support/Linux-i686"
  prefix="/usr/lib32"

  rm -rf "$outDir"
  mkdir -p "$outDir"

  cp "$prefix/libz.so.1" \
     "$prefix/libphysfs.so.1" \
     "$prefix/libSDL-1.2.so.0" \
     "$prefix/libSDL_ttf-2.0.so.0" \
     "$prefix/liblua.so.5.1" \
     "$prefix/libbz2.so.1.0" \
     "$prefix/libfreetype.so.6" \
     "$prefix/libogg.so.0" \
     "$prefix/libvorbis.so.0" \
     "$prefix/libvorbisfile.so.3" \
     "$prefix/libfreeimage.so.3" \
     "$outDir"

  cp "build/Linux-i686/src/txc_dxtn/libtxc_dxtn.so" \
     "$outDir"

  chmod +x "$outDir/"*
fi

if (( $Windows_i686 )); then
  outDir="support/Windows-i686"
  prefix="/usr/i486-mingw32/bin"

  rm -rf "$outDir"
  mkdir -p "$outDir"

  cp "$prefix/libgcc_s_sjlj-1.dll" \
     "$prefix/libstdc++-6.dll" \
     "$prefix/zlib1.dll" \
     "$prefix/libphysfs.dll" \
     "$prefix/SDL.dll" \
     "$prefix/SDL_ttf.dll" \
     "$prefix/lua52.dll" \
     "$prefix/libfreetype-6.dll" \
     "$prefix/libogg-0.dll" \
     "$prefix/libvorbis-0.dll" \
     "$prefix/libvorbisfile-3.dll" \
     "$prefix/FreeImage.dll" \
     "$outDir"

  chmod +x "$outDir/"*
fi
