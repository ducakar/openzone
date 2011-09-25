#!/bin/sh
#
# Some CMakeLists.txt files are auto-generated since some targets have lots of source files and it
# would be unconvenient to keep all CMakeLists.txt files up-to-date.
# Those CMakeLists.txt are generated via CMakeLists.gen scripts in the same directory that add all
# .hpp and .cpp files in the target directory to the target definition.
#

echo Generating src/oz/CMakeLists.txt
cd src/oz       && ./CMakeLists-gen.sh && cd ../..
echo Generating src/matrix/CMakeLists.txt
cd src/matrix   && ./CMakeLists-gen.sh && cd ../..
echo Generating src/nirvana/CMakeLists.txt
cd src/nirvana  && ./CMakeLists-gen.sh && cd ../..
echo Generating src/client/CMakeLists.txt
cd src/client   && ./CMakeLists-gen.sh && cd ../..
echo Generating src/server/CMakeLists.txt
cd src/server   && ./CMakeLists-gen.sh && cd ../..
