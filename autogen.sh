#!/bin/sh
#
# Some CMakeLists.txt files are auto-generated since some targets have lots of source files and it
# would be unconvenient to keep all CMakeLists.txt files up-to-date.
# Those CMakeLists.txt are generated via CMakeLists.gen scripts in the same directory that add all
# .hh and .cc files in the target directory to the target definition.
#

version="0.3.0"

components="oz common matrix nirvana client build"
conf_files="etc/liboz/liboz.pc etc/liboz/liboz.spec etc/openzone.spec"

for component in $components; do
  echo Generating src/$component/CMakeLists.txt
  ( cd src/$component && ./CMakeLists-gen.sh )
done

# Fix versions in various
for file in $conf_files; do
  echo Updating versions in $file
  sed "s/^\(Version: *\).*$/\1$version/g" -i $file
done
