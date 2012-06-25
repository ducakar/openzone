#!/bin/sh

baseDir=`dirname "$0"`
libsDir="$baseDir/support/Linux-`uname -m`"

LD_LIBRARY_PATH="$libsDir" exec "$baseDir/bin/ozBuild" -CA "data/ozbase"
LD_LIBRARY_PATH="$libsDir" exec "$baseDir/bin/ozBuild" -CA "data/openzone"
