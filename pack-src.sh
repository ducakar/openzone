#!/bin/sh

eval `grep '^version=' ./autogen.sh`
rel_files=`git ls-files`
topdir=`pwd`
topdir_name=`basename $topdir`

cd ..
for file in $rel_files; do
  files="$files $topdir_name/$file"
done

files="$files $topdir_name/share/applications"
files="$files $topdir_name/share/pixmaps"

echo Packing openzone-src-$version.tar.xz
tar Jcf $topdir_name/openzone-src-$version.tar.xz $files

cd $topdir

echo Packing openzone-data-ozbase-$version.tar.xz
#tar Jcf openzone-data-ozbase-$version.tar.xz -C share/openzone ozbase.zip

echo Packing openzone-data-openzone-$version.tar.xz
#tar Jcf openzone-data-openzone-$version.tar.xz -C share/openzone openzone.zip
