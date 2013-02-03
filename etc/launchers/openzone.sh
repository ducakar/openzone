#!/bin/sh

baseDir=`dirname "$0"`
arch=Linux-i686
[[ `uname -m` == x86_64 ]] && arch=Linux-x86_64

cd "$baseDir" && exec ./bin/$arch/openzone -p . $@
