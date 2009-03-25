#!/bin/sh

FILES="src/*/*.h src/*/*.cpp src/*/*.gen *.h src/*/CMakeLists.txt src/CMakeLists.txt CMakeLists.txt *.sh"

sed -i -e 's/[	 ]\+$//' $FILES
