#!/bin/sh

echo Generating src/base/CMakeLists.txt
cd src/base    && ./CMakeLists.gen && cd ../..
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
