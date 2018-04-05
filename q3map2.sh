#!/bin/bash
#
# q3map2.sh <map_file>
#
# Proxy script for invoking `q3map2` BSP compiler from GtkRadiant in with appropriate parameters to
# compile a BSP structures for OpenZone.
#

if [[ -z $1 ]]; then
  echo "Usage: $0 <map_file>"
  exit
fi

radiantHome=/usr/share/gtkradiant
basePath=`echo $1 | sed -r 's|(.*)/baseq3/maps/.*|\1|'`

"$radiantHome"/q3map2 -fs_basepath "$basePath" -meta -v $1
