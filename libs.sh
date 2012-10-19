#!/bin/sh

platforms=( Linux-x86_64 Linux-i686 Windows-i686 )

function clean()
{
  for platform in ${platforms[@]}; do
    rm -rf libs/$platform
  done
}

function build()
{
  for platform in ${platforms[@]}; do
    if [[ $platform == Linux-x86_64 || $platform == Linux-i686 ]]; then
      outDir="libs/$platform"
      prefix="/usr/lib"

      [[ $platform == Linux-x86_64 && ! -d "/usr/lib32" ]] && prefix="/usr/lib64"
      [[ $platform == Linux-i686   &&   -d "/usr/lib32" ]] && prefix="/usr/lib32"

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

      cp "build/$platform/src/txc_dxtn/libtxc_dxtn.so" \
        "$outDir"

      chmod +x "$outDir/"*
      strip "$outDir"/*
    fi

    if [[ $platform == Windows-i686 ]]; then
      outDir="libs/Windows-i686"
      prefix="/usr/i486-mingw32"

      rm -rf "$outDir"
      mkdir -p "$outDir"

      cp "$prefix/bin/libgcc_s_sjlj-1.dll" \
        "$prefix/bin/libstdc++-6.dll" \
        "$prefix/lib/pthreadGC2.dll" \
        "$prefix/bin/zlib1.dll" \
        "$prefix/bin/libphysfs.dll" \
        "$prefix/bin/SDL.dll" \
        "$prefix/bin/SDL_ttf.dll" \
        "$prefix/bin/lua52.dll" \
        "$prefix/bin/libfreetype-6.dll" \
        "$prefix/bin/libogg-0.dll" \
        "$prefix/bin/libvorbis-0.dll" \
        "$prefix/bin/libvorbisfile-3.dll" \
        "$prefix/bin/FreeImage.dll" \
        "$outDir"

      chmod +x "$outDir/"*
      i486-mingw32-strip "$outDir"/*
    fi
  done
}

case "$1" in
  clean)
    clean
    ;;
  build|*)
    build
    ;;
esac
