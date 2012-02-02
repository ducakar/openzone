#!/bin/sh

eval `grep '^version=' ./autogen.sh`
rel_files=`git ls-files`
topdir=`pwd`
topdir_name=`basename $topdir`

cd ..
for file in $rel_files; do
  files="$files $topdir_name/$file"
done

tar Jcf $topdir_name/openzone-src-$version.tar.xz $files
