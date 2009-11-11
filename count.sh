#!/bin/sh

FILES="src/*/*.h src/*/*.cpp precompiled.h src/*/CMakeLists.gen src/CMakeLists.txt"
FILES="$FILES data/CMakeLists.txt data/*/*.rc"
FILES="$FILES data/lua/*/*.lua data/lua/*/CMakeLists.gen data/lua/CMakeLists.txt"
FILES="$FILES CMakeLists.txt *.sh"

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
