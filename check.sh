#!/bin/sh

cppcheck --template '[{file}:{line}]: ({severity}:{id}) {message}' --enable=all \
    --suppress=uninitVar --suppress=noConstructor -Isrc \
    -ILinux-x86_64-Clang/src/oz -ILinux-x86_64-Clang/src src
