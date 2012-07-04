#!/bin/sh

cat << EOF > CMakeLists.txt
add_library( modules STATIC
  `echo *.cc */*.cc | sed 's/ /\n  /g'` )
add_dependencies( modules pch )
EOF
