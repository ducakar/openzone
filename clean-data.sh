#!/bin/sh

[[ -z "$1" ]] && exit

dir="$1"

echo rm -f $dir'/lingua/*/*.mo'
rm -f $dir/lingua/*/*.mo
echo rm -f $dir'/baseq3/maps/*{autosave.map,.bak,.prt,.srf}'
rm -f $dir/baseq3/maps/*{autosave.map,.bak,.prt,.srf}
echo rm -f $dir'/mdl/*.{blend1,blend2}'
rm -f $dir/mdl/*.{blend1,blend2}
