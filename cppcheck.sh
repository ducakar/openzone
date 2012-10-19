#!/bin/sh

cppcheck --enable=all --force -Isrc -Ibuild/Linux-x86_64-Clang/src/oz \
         -Ibuild/Linux-x86_64-Clang/src src &> cppcheck.log
