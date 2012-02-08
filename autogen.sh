#!/bin/sh
#
# Some CMakeLists.txt files are auto-generated since some targets have lots of source files and it
# would be unconvenient to keep all CMakeLists.txt files up-to-date.
# Those CMakeLists.txt are generated via CMakeLists.gen scripts in the same directory that add all
# .hh and .cc files in the target directory to the target definition.
#

version="0.2.80"

components="oz common matrix nirvana modules client build"
conf_files="etc/liboz/liboz.spec etc/openzone.spec"

for component in $components; do
  echo Generating src/$component/CMakeLists.txt
  ( cd src/$component && ./CMakeLists-gen.sh )
done

# Fix versions in various files
for file in $conf_files; do
  echo Updating versions in $file
  sed "s/^\(Version: *\).*$/\1$version/g" -i $file
done

echo Updating PKGBUILD
sed "s/^\(pkgver=*\).*$/\1$version/g" -i etc/PKGBUILD
sed "s/^\(_dataver=*\).*$/\1$version/g" -i etc/PKGBUILD

echo Updating version in Doxygen
sed "s/\(PROJECT_NUMBER *= \).*$/\1$version/" -i etc/liboz/Doxyfile etc/Doxyfile

echo Updating version in CMakeLists.txt
sed "s/^set( OZ_APPLICATION_VERSION .*$/set( OZ_APPLICATION_VERSION \"$version\" CACHE STRING \"\" FORCE )/" -i CMakeLists.txt
