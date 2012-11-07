#!/bin/sh

if [[ ! -d data/ozbase ]]; then
  echo "Directory ./data/ozbase does not exist."
  exit
fi

outFile="data/ozbase/lingua/ozbase.pot"

rm -rf "$outFile"
xgettext --omit-header -C -s -kOZ_GETTEXT -o "$outFile" \
         src/*/*.{hh,cc} src/*/*/*.{hh,cc}

cat << EOF >> "$outFile"

msgid ""
`cat data/ozbase/credits/ozbase.txt | sed -r 's|^(.*)$|"\1\\\\n"|'`
msgstr ""
EOF
