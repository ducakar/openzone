#!/bin/sh
#
# Generate SLOC statistics with sloccount and cloc.
#

which sloccount &> /dev/null && LANG=C sloccount src

echo
echo

which cloc &> /dev/null && cloc --force-lang=C++,hh src cmake *.txt *.sh
