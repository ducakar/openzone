#!/bin/sh

sources='src/*/*.hh src/*/*/*.hh src/*/*.cc src/*/*/*.cc'
output='share/openzone/lingua/ozbase.pot'

rm -rf $output
xgettext --omit-header -C -s -k_ -d ozbase -o $output $sources
