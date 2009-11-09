#!/bin/sh

FILES="src/*/*.h src/*/*.cpp src/*/*.gen *.h data/lua/*.lua data/*/*.rc src/CMakeLists.txt CMakeLists.txt *.sh"

wc -lc $FILES

(( nFiles = 0 ))
for file in $FILES; do
  (( nFiles++ ));
done

echo
echo $nFiles files

unset nFiles
unset FILES

if [ -x /usr/bin/sloccount ]; then
  LANG=C /usr/bin/sloccount src *.h *.sh
fi
