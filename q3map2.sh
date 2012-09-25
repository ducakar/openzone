#!/bin/sh

if [[ -z "$1" ]]; then
  echo "Usage: $0 <map_file>"
  exit
fi

basepath=`echo $1 | sed 's|\(.*\)/baseq3/maps/.*|\1|'`

/usr/share/gtkradiant/q3map2 -fs_basepath "$basepath" -meta -v $1
