#!/bin/sh

sed '/./,/^$/ !d' -i `git ls-files | grep -v '^include'`
Linux-x86_64-Clang/src/tools/checkBlanks `git ls-files | grep -v '^include'`
