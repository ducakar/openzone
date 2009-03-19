#!/bin/sh

cd src/base    && ./CMakeLists.gen && cd ../..
cd src/matrix  && ./CMakeLists.gen && cd ../..
cd src/nirvana && ./CMakeLists.gen && cd ../..
cd src/client  && ./CMakeLists.gen && cd ../..
cd src/server  && ./CMakeLists.gen && cd ../..
