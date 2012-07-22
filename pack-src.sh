#!/bin/sh

eval `grep '^version=' ./autogen.sh`
rel_files=`git ls-files`
topdir=`pwd`
topdir_name=`basename $topdir`

cd "$topdir/.."
for file in $rel_files; do
  files="$files $topdir_name/$file"
done

files="$files $topdir_name/share/applications"
files="$files $topdir_name/share/pixmaps"

echo "Packing openzone-src-$version.tar.xz"
tar Jcf "$topdir/openzone-src-$version.tar.xz" $files

cd "$topdir/share"

echo "Packing openzone-data-$version.tar.xz"
tar Jcf "$topdir/openzone-data-$version.tar.xz" openzone/*.zip
