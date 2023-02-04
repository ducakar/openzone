#!/bin/bash
#
# gen-manifest.sh
#
# Writes `share/openzone/manifest.json` file that contains list of game packeges together with their
# timestamps. Needed by NaCl to update cached game packages.
#

set -e

cd share/openzone

echo '{' >manifest.json

first=1

for pkg in *.zip; do
  if [[ -f "$pkg" ]]; then
    timestamp=$(stat -c %Y "$pkg")

    if [[ -n $first ]]; then
      unset first
    else
      echo ',' >>manifest.json
    fi
    echo -n "  \"$pkg\": $timestamp" >>manifest.json
  fi
done

echo -e '\n}' >>manifest.json

cat manifest.json
