#!/bin/sh

cppcheck --template '[{file}:{line}]: ({severity}:{id}) {message}' --enable=all \
         -Isrc -Ibuild/Linux-x86_64-Clang/src/oz -Ibuild/Linux-x86_64-Clang/src \
         src
