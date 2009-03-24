#!/bin/sh

FILES="src/*/*.h src/*/*.cpp src/*/*.gen *.h *.sh CMakeLists.txt src/CMakeLists.txt src/*/CMakeLists.txt"

sed -i -e 's/[	 ]\+$//' $FILES
