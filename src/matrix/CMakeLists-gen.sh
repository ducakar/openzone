#!/bin/sh

cat << EOF > CMakeLists.txt
add_library( matrix STATIC
  `echo *.{hh,cc} | sed 's/ /\n  /g'` )
use_pch( matrix pch )
EOF
