#!/bin/sh
#
# Some CMakeLists.txt files are auto-generated since some targets have lots of source files and it
# would be unconvenient to keep all CMakeLists.txt files up-to-date.
# Those CMakeLists.txt are generated via CMakeLists.gen scripts in the same directory that add all
# .hpp and .cpp files in the target directory to the target definition.
#

components='oz common matrix nirvana client build'

for component in $components; do
  echo Generating src/$component/CMakeLists.txt
  ( cd src/$component && ./CMakeLists-gen.sh )
done
