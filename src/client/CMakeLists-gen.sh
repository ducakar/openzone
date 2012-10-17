#!/bin/sh

cat << EOF > CMakeLists.txt
add_library( client STATIC
  `echo *.{hh,cc} ui/*.{hh,cc} | sed 's| |\n  |g'` )
use_pch( client pch )
EOF
