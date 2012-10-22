#!/bin/sh

cat << EOF > CMakeLists.txt
add_library( builder STATIC
  `echo *.{hh,cc} | sed 's| |\n  |g'` )
use_pch( builder pch )
EOF
