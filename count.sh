#!/bin/sh

if [ -x /usr/bin/sloccount ]; then
  LANG=C /usr/bin/sloccount src
fi

if [ -x /usr/bin/cloc ]; then
  /usr/bin/cloc --exclude-ext=gen src share/openzone/lua share/openzone/glsl
fi
