#!/bin/sh
#
# First, count lines and bytes in different groups of files (C++ sources, hand-written data text
# files (Lua, .rc files) and hand-written buildsystem files) then, if sloccount in found on
# system, count SLOC in C++ source files. 
#

source=`echo src/*/*.{hpp,cpp} src/stable.hpp`
lua=`echo share/openzone/lua/*/*.lua`
glsl=`echo share/openzone/glsl/*.{glsl,vert,frag}`
data=`echo share/openzone/*/*.rc`
build=`echo src/*/CMakeLists.gen {src,share/openzone,share/openzone/lua}/*/CMakeLists.txt src/*/*.in src/*.in CMakeLists.txt *.sh`

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

count 'C++ source' $source
count 'Lua scripts' $lua
count 'Shader source' $glsl
count 'Data config' $data
count 'Build system etc.' $build

if [ -x /usr/bin/sloccount ]; then
  LANG=C /usr/bin/sloccount src
fi

if [ -x /usr/bin/cloc ]; then
  /usr/bin/cloc --exclude-ext=gen src share/openzone/lua share/openzone/glsl
fi
