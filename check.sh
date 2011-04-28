#!/bin/sh

cppcheck --template '[{file}:{line}]: ({severity}:{id}) {message}' --enable=all \
    --suppress=uninitVar --suppress=noConstructor -q -Isrc src
