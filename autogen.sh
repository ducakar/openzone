#!/bin/sh
#
# Some CMakeLists.txt files are auto-generated since some targets have lots of source files and it
# would be hard to keep them all consistent and up-to-date manually.
# Those CMakeLists.txt are generated with CMakeLists-gen.sh scripts in the same directory that add
# all .hh and .cc files in the target directory to the target definition.
# Additionaly this scripts also updates version numbers in variout files.
#

version="0.2.90"
components="oz common matrix nirvana modules client build unittest"

# Generate CMakeLists.txt files.
for component in $components; do
  echo "Generating src/$component/CMakeLists.txt"
  ( cd src/$component && ./CMakeLists-gen.sh )
done

# Fix version numbers.
echo "Updating version in CMakeLists.txt"
sed 's/^\(set( OZ_VERSION "\)[^"]*\(".*\)$/\1'"$version"'\2/' -i CMakeLists.txt

echo "Updating version in doc/Doxyfiles.liboz and doc/Doxyfile"
sed 's/^\(PROJECT_NUMBER *= \).*$/\1'"$version"'/' -i doc/Doxyfile.liboz doc/Doxyfile

echo "Updating version in etc/PKGBUILD"
sed 's/^\(pkgver=\).*$/\1'"$version"'/' -i etc/PKGBUILD

echo "Updating version in etc/openzone.spec"
sed 's/^\(Version: *\).*$/\1'"$version"'/' -i etc/openzone.spec
