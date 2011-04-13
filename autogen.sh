#!/bin/sh
#
# Some CMakeLists.txt files are auto-generated since some targets have lots of source files and it
# would be unconvenient to keep all CMakeLists.txt files up-to-date.
# Those CMakeLists.txt are generated via CMakeLists.gen scripts in the same directory that add all
# .hpp and .cpp files in the target directory to the target definition.
#

export LC_COLLATE=C

echo Generating src/oz/CMakeLists.txt
cd src/oz      && ./CMakeLists-gen.sh && cd ../..
echo Generating src/matrix/CMakeLists.txt
cd src/matrix  && ./CMakeLists-gen.sh && cd ../..
echo Generating src/nirvana/CMakeLists.txt
cd src/nirvana && ./CMakeLists-gen.sh && cd ../..
echo Generating src/arch/CMakeLists.txt
cd src/arch &&    ./CMakeLists-gen.sh && cd ../..
echo Generating src/client/CMakeLists.txt
cd src/client  && ./CMakeLists-gen.sh && cd ../..
echo Generating src/server/CMakeLists.txt
cd src/server  && ./CMakeLists-gen.sh && cd ../..

#
# For the same reason as CMakeLists.txt we also automatically generate install_files.cmake file
# that includes files that should be installed for each install target.
#

echo Generating install_files.cmake
./install_files-gen.sh

#
# generate .pot gettext translations template
#
echo Generating gettext.pot
./gettext-gen.sh
