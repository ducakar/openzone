#!/bin/sh

FILES="src/*/*.hpp src/*/*.cpp *.sh CMakeLists.txt src/CMakeLists.txt src/*/CMakeLists.txt"

wc -cl $FILES

(( nFiles = 0 ))
for file in $FILES; do
  (( nFiles++ ));
done

echo
echo $nFiles files

unset nFiles
unset FILES

if [ -x /usr/bin/sloccount ]; then
  LANG=C /usr/bin/sloccount src
fi
