#!/bin/sh

if [ -x /usr/bin/sloccount ]; then
  LANG=C /usr/bin/sloccount src
fi

echo
echo

if [ -x /usr/bin/cloc ]; then
  /usr/bin/cloc --quiet --force-lang=C++,hh --force-lang=C++,in --exclude-lang=XML --exclude-ext=h \
    `git ls-files`
fi
