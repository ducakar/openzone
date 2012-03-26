#!/bin/sh

if [[ ! -d data/ozbase ]]; then
  echo "Directory ./data/ozbase does not exist."
  exit
fi

sources='src/*/*.hh src/*/*/*.hh src/*/*.cc src/*/*/*.cc'
output='data/ozbase/lingua/ozbase.pot'

rm -rf $output
xgettext --omit-header -C -s -kOZ_GETTEXT -d ozbase -o $output $sources
