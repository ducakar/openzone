#!/bin/sh

eval `grep '^version=' ./autogen.sh`
rel_files=`git ls-files | grep -v '^data/'`
topdir=`pwd`
topdir_name=`basename $topdir`

cd "$topdir/.."
for file in $rel_files; do
  files="$files $topdir_name/$file"
done

files="$files $topdir_name/share/applications"
files="$files $topdir_name/share/pixmaps"

if [[ "$1" != "data" ]]; then
  echo "Packing openzone-src-$version.tar.xz"
  tar Jcf "$topdir/openzone-src-$version.tar.xz" $files
fi

if [[ "$1" != "src" ]]; then
  echo "Packing openzone-data-$version.tar.xz"
  tar Jcf "$topdir/openzone-data-$version.tar.xz" openzone/share/openzone/*.zip

  echo "Packing openzone-data-src-$version.tar.xz"
  tar Jcf "$topdir/openzone-data-src-$version.tar.xz" --exclude=DISABLED openzone/data
fi
