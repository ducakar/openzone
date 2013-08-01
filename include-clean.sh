#!/bin/sh

iwyuPath=/home/davorin/Prenosi/include-what-you-use/build
build=Linux-`uname -m`-Clang

if (( $# == 1 )); then
  "$iwyuPath"/include-what-you-use -Xiwyu --transitive_includes_only -Xiwyu --verbose=1 \
                                   -std=c++11 -isystem /usr/lib/clang/3.3/include \
                                   -I /home/davorin/Razvoj/openzone/build/$build/src \
                                   -I /home/davorin/Razvoj/openzone/src $1
else
  for file in $@; do
    "$iwyuPath"/include-what-you-use -Xiwyu --transitive_includes_only -Xiwyu --verbose=1 \
                                     -std=c++11 -isystem /usr/lib/clang/3.3/include \
                                     -I /home/davorin/Razvoj/openzone/build/$build/src \
                                     -I /home/davorin/Razvoj/openzone/src $file \
                                     |& sed '/.*should remove.*/,/^$/ !d'
  done
fi
