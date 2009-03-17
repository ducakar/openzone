#!/bin/sh

FILES="src/*.h src/*/*.h src/*/*.cpp *.sh CMakeLists.txt src/CMakeLists.txt src/*/CMakeLists.txt"

sed -i -e 's/[	 ]\+$//' $FILES
