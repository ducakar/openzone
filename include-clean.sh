#!/bin/sh

build=Linux-`uname -m`-Clang

for file in $@; do
  include-what-you-use -Xiwyu --transitive_includes_only \
                        -std=c++11 -isystem /usr/lib/clang/3.3/include \
                        -I /home/davorin/Razvoj/openzone/build/$build/src \
                        -I /home/davorin/Razvoj/openzone/src $file \
                        |& sed '/.*should remove.*/,/^$/ !d'
done
