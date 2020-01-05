#!/bin/bash
#
# gen-manifest.sh
#
# Writes `share/openzone/manifest.json` file that contains list of game packeges together with their
# timestamps. Needed by NaCl to update cached game packages.
#

set -e

cd share/openzone

printf '{\n' > manifest.json

first=1

for pkg in `echo *.zip`; do
  if [[ -f $pkg ]]; then
    timestamp=`stat -c %Y $pkg`

    (( $first )) && first=0 || ( printf ',\n' >> manifest.json )
    printf "  \"$pkg\": $timestamp" >> manifest.json
  fi
done

printf '\n}\n' >> manifest.json

cat manifest.json
