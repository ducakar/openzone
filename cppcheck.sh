#!/bin/bash
#
# cppcheck.sh
#
# Runs cppcheck tool for static code analysis. Output is written into `cppcheck.log`.
#

set -e

path="src"
[[ -n $1 ]] && path="$1"

cppcheck -j 4 --enable=all --force -Isrc -Ibuild/Linux-x86_64-Clang/src/ozCore \
  -Ibuild/Linux-x86_64-Clang/src "$path" |& grep '^\[' &>cppcheck.log
