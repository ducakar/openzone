#!/bin/sh

FILES="src/*/*.h src/*/*.cpp *.sh CMakeLists.txt src/CMakeLists.txt src/*/CMakeLists.txt"

wc -cl $FILES

(( nFiles = 0 ))
for file in $FILES; do
  (( nFiles++ ));
done

echo
echo $nFiles files

unset nFiles
unset FILES
