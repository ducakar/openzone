#!/bin/sh

#if [ -x /usr/bin/sloccount ]; then
#  LANG=C /usr/bin/sloccount src
#fi

if [ -x /usr/bin/cloc ]; then
  /usr/bin/cloc --force-lang=C++,hpp --force-lang=C++,in --exclude-lang=XML --exclude-ext=h `git ls-files`
fi
