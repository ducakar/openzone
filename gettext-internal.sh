#!/bin/sh
#
# gettext-internal.sh <data_dir>
#
# Extracts internal strings for localisation from OpenZone source code and appends them to
# `<data_dir>/lingua/<data_dir_basename>.pot`.
#

if [[ -z $1 ]]; then
  echo "Usage: $0 <data_dir>"
  exit
fi

if [[ ! -d $1 ]]; then
  echo "Directory $1 does not exist."
  exit
fi

outFile="$1/lingua/`basename $1`.pot"

echo "Appending extracted strings to $outFile"

echo '' >> "$outFile"
xgettext --omit-header -C -s -kOZ_GETTEXT -o - src/*/*.{hh,cc} src/*/*/*.{hh,cc} >> "$outFile"
