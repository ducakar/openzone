#!/bin/sh

if [[ ! -d data/ozbase ]]; then
  echo "Directory ./data/ozbase does not exist."
  exit
fi

rm -rf data/ozbase/lingua/ozbase.pot
xgettext --omit-header -C -s -kOZ_GETTEXT -o data/ozbase/lingua/ozbase.pot \
         src/*/*.{hh,cc} src/*/*/*.{hh,cc}
