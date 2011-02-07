#!/bin/sh
#
# First, count lines and bytes in different groups of files (C++ sources, hand-written data text
# files (Lua, .rc files) and hand-written buildsystem files) then, if sloccount in found on
# system, count SLOC in C++ source files. 
#

source=`echo src/*/*.{hpp,cpp} src/stable.hpp`
data=`echo data/*/*.rc data/lua/*/*.lua`
build=`echo src/*/CMakeLists.gen {src,data}/*/CMakeLists.txt src/*/*.in src/*.in CMakeLists.txt *.sh`

function count()
{
  printf '%s (' "$1"
  shift
  (( nFiles = 0 ))
  for file in $@; do (( nFiles++ )); done
  printf '%d files):\n' $nFiles
  wc -lc $@
  echo
}

#count 'C++ Source' $source
#count 'Data config & Lua scripts' $data
#count 'Build system etc.' $build

#if [ -x /usr/bin/sloccount ]; then
#  LANG=C /usr/bin/sloccount src
#fi

if [ -x /usr/bin/cloc ]; then
  /usr/bin/cloc --exclude-ext=gen src data/lua
fi
