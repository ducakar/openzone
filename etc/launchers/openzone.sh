#!/bin/sh

baseDir=`dirname "$0"`
libsDir="$baseDir/support/Linux-`uname -m`"

LD_LIBRARY_PATH="$libsDir" exec "$baseDir/bin/openzone" $@
