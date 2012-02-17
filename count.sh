#!/bin/sh

which sloccount &> /dev/null && LANG=C sloccount src

echo
echo

which cloc &> /dev/null && cloc --quiet --force-lang=C++,hh --force-lang=C++,in \
                                --exclude-lang=XML --exclude-ext=h `git ls-files`
