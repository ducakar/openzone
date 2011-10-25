#!/bin/sh

cppcheck --template '[{file}:{line}]: ({severity}:{id}) {message}' --enable=all \
    --suppress=uninitVar --suppress=noConstructor -DOZ_POOL_ALLOC -Isrc -j4 src
