#!/bin/sh

if [[ `uname -m` == "x86_64" ]]; then
  arch="Linux-x86_64"
  elfInterpreter="/lib64/ld-linux-x86-64.so.2"
else
  arch="Linux-i686"
  elfInterpreter="/lib/ld-linux.so.2"
fi
baseDir=`dirname "$0"`

cd "$baseDir"
LD_LIBRARY_PATH="./lib/$arch" exec "$elfInterpreter" "./bin/$arch/openzone" -p "." $@
