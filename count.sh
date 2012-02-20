#!/bin/sh

files=`git ls-files | grep -v '^include'`

which sloccount &> /dev/null && LANG=C sloccount $files

echo
echo

which cloc &> /dev/null && cloc --quiet --force-lang=C++,hh --exclude-lang=XML $files
