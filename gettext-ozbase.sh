#!/bin/sh

sources='src/*/*.hh src/*/*/*.hh src/*/*.cc src/*/*/*.cc'
output='pkg/build/ozbase/lingua/ozbase.pot'

rm -rf $output
xgettext --omit-header -C -s -kOZ_GETTEXT -d ozbase -o $output $sources