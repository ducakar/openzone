#!/bin/sh
#
# cppcheck.sh
#
# Runs cppcheck tool for static code analysis. Output is written into `cppcheck.log`.
#

cppcheck --enable=all --force -Isrc -Ibuild/Linux-x86_64-Clang/src/ozCore \
         -Ibuild/Linux-x86_64-Clang/src src &> cppcheck.log
