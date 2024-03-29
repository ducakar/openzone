#!/bin/bash
#
# q3map2.sh <map_file>
#
# Proxy script for invoking `q3map2` BSP compiler from GtkRadiant in with appropriate parameters to
# compile a BSP structures for OpenZone.
#

set -e

if [[ -z $1 ]]; then
  echo "Usage: $0 <map_file>"
  exit
fi

radiantHome=/opt/gtkradiant
# radiantHome=/usr/bin
basePath=$(echo "$1" | sed -E 's|(.*)/baseq3/maps/.*|\1|')

"$radiantHome"/q3map2 -fs_basepath "$basePath" -meta -v "$1"
