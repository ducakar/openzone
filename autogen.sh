#!/bin/bash
#
# autogen.sh
#
# Some targets have lots of source files and it would be hard to keep them all consistent and
# up-to-date manually. Lists of source files in corresponding CMakeLists.txt are thus generated by
# this script.
#
# Additionally this scripts updates version numbers in various files.
#

components=(src/ozCore src/ozEngine src/ozFactory src/unittest
            src/common src/matrix src/nirvana src/client src/builder)
version=`sed -r '/^set\(OZ_VERSION / !d; s|.* ([0-9.]+)\)|\1|' CMakeLists.txt`

# Generate CMakeLists.txt files.
for component in ${components[@]}; do
  echo "Generating $component/CMakeLists.txt"

  cd $component

  sources=`echo *.{hh,cc} */*.{hh,cc}`
  # Remove uninstantiated *.hh, *.cc, */*.hh and */*.cc expressions.
  sources=`echo $sources | sed -r 's|(\*/)?\*\.[^ ]*||g'`
  # Make file list newline-separated and indented.
  sources=`echo $sources | sed -r 's| |\\\\n  |g'`

  # Insert source file list between "#BEGIN SOURCES" and "#END SOURCES" tags in CMakeLists.txt.
  sed -r '/^#BEGIN SOURCES$/,/^#END SOURCES$/ c\#BEGIN SOURCES\n  '"$sources"'\n#END SOURCES' \
      -i CMakeLists.txt

  cd -
done

# Fix version numbers.
echo "Updating version in doc/Doxyfile*"
sed -r 's|^(PROJECT_NUMBER *= *).*$|\1"'"$version"'"|' -i doc/Doxyfile*

echo "Updating HTML READMEs doc/*.html"
sed -r 's|(<!--OZ_VERSION-->)[^<"]*|\1'"$version"'|' -i doc/*.html

echo "Updating version in etc/openzone.spec"
sed -r 's|^(Version: *).*$|\1'"$version"'|' -i etc/openzone.spec

echo "Updating version in etc/PKGBUILD*"
sed -r 's|^(pkgver=).*$|\1'"$version"'|' -i etc/PKGBUILD*
