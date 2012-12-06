#!/bin/sh
#
# gettext-base.sh <basedata_dir>
#
# Creates `<basedata_dir>/<basename>.pot` template for localised messages catalogue. See `ozGettext`
# tool for more details. `<basename>` is package name (last directory name in `<basedata_dir>`).
#

if [[ -z $1 ]]; then
  echo "Usage: $0 <basedata_dir>"
  exit
fi

if [[ ! -d $1 ]]; then
  echo "Directory $1 does not exist."
  exit
fi

baseName=`basename $1`
outFile="$1/lingua/$baseName.pot"

rm -rf $outFile
xgettext --omit-header -C -s -kOZ_GETTEXT -o $outFile \
         src/*/*.{hh,cc} src/*/*/*.{hh,cc}

cat << EOF >> $outFile

msgid ""
`cat $1/credits/$baseName.txt | sed -r 's|^(.*)$|"\1\\\\n"|'`
msgstr ""
EOF
