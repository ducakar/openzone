#!/bin/sh

FILES="src/*/*.h src/*/*.cpp src/*/*.gen *.h *.sh CMakeLists.txt src/CMakeLists.txt"

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
  LANG=C /usr/bin/sloccount src/base src/matrix src/nirvana src/client src/server src/test *.h *.sh
fi
