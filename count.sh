#!/bin/sh

FILES="src/*/*.h src/*/*.cpp src/*/*.gen *.h data/*/*.xml src/CMakeLists.txt CMakeLists.txt *.sh"

wc -l $FILES

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
