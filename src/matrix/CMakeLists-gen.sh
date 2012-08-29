#!/bin/sh

cat << EOF > CMakeLists.txt
add_library( matrix STATIC
  `echo *.{hh,cc} | sed 's/ /\n  /g'` )
add_dependencies( matrix pch )
EOF
