#!/bin/sh
#
# Generate SLOC statistics with sloccount and cloc.
#

files=`git ls-files | grep -v '^include'`

which sloccount &> /dev/null && LANG=C sloccount src/* *.sh

echo
echo

which cloc &> /dev/null && 
  cloc --quiet --force-lang=C++,hh --force-lang=C++,h \
       --exclude-lang='ASP.Net','HTML','Teamcenter def','XML' $files
