#!/bin/sh
#
# Clean-up package source directory.
# This removes temporary, intermediate and backup files that are created by some programs while
# editing game data.
#
# Usage:
#   ./clean-data.sh <datasrc_dir>
#

if [[ -z "$1" ]]; then
  echo "Usage: $0 <datasrc_dir>"
  exit
fi

dir="$1"

echo "rm -vf $dir/lingua/*/*.mo $dir/mission/*/lingua/*.mo"
rm -vf "$dir"/lingua/*/*.mo "$dir"/mission/*/lingua/*.mo
echo "rm -vf "$dir"/baseq3/maps/*{autosave.map,.bak,.prt,.srf}"
rm -vf "$dir"/baseq3/maps/*{autosave.map,.bak,.prt,.srf,autosave.bsp}
echo "rm -vf $dir/mdl/*.{blend1,blend2}"
rm -vf "$dir"/mdl/*.{blend1,blend2}
