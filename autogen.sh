#!/bin/sh
#
# Some CMakeLists.txt files are auto-generated since some targets have lots of source files and it
# would be unconvenient to keep all CMakeLists.txt files up-to-date.
# Those CMakeLists.txt are generated via CMakeLists.gen scripts in the same directory that add all
# .hh and .cc files in the target directory to the target definition.
#

version="0.2.80"

components="oz common matrix nirvana modules client build"

for component in $components; do
  echo Generating src/$component/CMakeLists.txt
  ( cd src/$component && ./CMakeLists-gen.sh )
done

# Fix versions in various files
echo Updating version in CMakeLists.txt
sed "s/^set( OZ_APPLICATION_VERSION .*$/set( OZ_APPLICATION_VERSION \"$version\" CACHE STRING \"\" FORCE )/" -i CMakeLists.txt

echo Updating version in doc/Doxyfiles.liboz and doc/Doxyfile
sed "s/\(PROJECT_NUMBER *= \).*$/\1$version/" -i doc/Doxyfile.liboz doc/Doxyfile

echo Updating version in etc/openzone.spec
sed "s/^\(Version: *\).*$/\1$version/g" -i etc/openzone.spec

echo Updating version in etc/PKGBUILD
sed "s/^\(pkgver=*\).*$/\1$version/g" -i etc/PKGBUILD
