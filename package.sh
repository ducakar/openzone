#!/bin/sh

eval `grep '^version=' ./autogen.sh`

files=`git ls-files | grep -v '^data/'`
files="$files share/applications share/pixmaps"

if [[ $1 != data ]]; then
  echo "Packing openzone-src-$version.tar.xz"
  tar Jcf "openzone-src-$version.tar.xz" --xform "s|^|openzone-$version/|" $files
fi

if [[ $1 != src ]]; then
  echo "Packing openzone-data-$version.tar.xz"
  tar Jcf "openzone-data-$version.tar.xz" --xform "s|^|openzone-$version/|" share/openzone/*.zip

  echo "Packing openzone-data-src-$version.tar.xz"
  tar Jcf "openzone-data-src-$version.tar.xz" --xform "s|^|openzone-$version/|" --exclude=DISABLED data
fi
