#!/bin/bash
#
# gettext-internal.sh <data_dir>
#
# Extracts internal strings for localisation from OpenZone source code and appends them to
# `<data_dir>/lingua/<data_dir_basename>.pot`.
#

set -e

outFile="data/ozBase/lingua/internal.pot"

echo "Extracting strings to $outFile"

echo >"$outFile"
xgettext --omit-header -C -s -kOZ_GETTEXT -o - src/*/*.{hh,cc} src/*/*/*.{hh,cc} >>"$outFile"
