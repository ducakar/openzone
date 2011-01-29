#!/bin/sh
#
# Some CMakeLists.txt files are auto-generated since some targets have lots of source files and it
# would be unconvenient to keep all CMakeLists.txt files up-to-date.
# Those CMakeLists.txt are generated via CMakeLists.gen scripts in the same directory that add all
# .hpp and .cpp files in the target directory to the target definition.
#

export LC_COLLATE=C

echo Generating src/oz/CMakeLists.txt
cd src/oz      && ./CMakeLists.gen && cd ../..
echo Generating src/matrix/CMakeLists.txt
cd src/matrix  && ./CMakeLists.gen && cd ../..
echo Generating src/nirvana/CMakeLists.txt
cd src/nirvana && ./CMakeLists.gen && cd ../..
echo Generating src/ui/CMakeLists.txt
cd src/ui      && ./CMakeLists.gen && cd ../..
echo Generating src/client/CMakeLists.txt
cd src/client  && ./CMakeLists.gen && cd ../..
echo Generating src/server/CMakeLists.txt
cd src/server  && ./CMakeLists.gen && cd ../..

echo Generating data/lua/matrix/CMakeLists.txt
cd data/lua/matrix  && ./CMakeLists.gen && cd ../../..
echo Generating data/lua/nirvana/CMakeLists.txt
cd data/lua/nirvana && ./CMakeLists.gen && cd ../../..

#
# For the same reason as CMakeLists.txt we also automatically generate install_files.cmake file
# that includes files that should be installed for each install target.
#

echo Generating install_files.cmake
./install_files.gen
