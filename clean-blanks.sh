#!/bin/sh

files=`git ls-files | grep -v '^include' | grep -v 'README' | grep -v 'header\.glsl'`

for file in $files; do
  sed '/./,/^$/ !d' -i $file
  Linux-x86_64-Clang/src/tools/checkBlanks $file
done
