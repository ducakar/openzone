#!/bin/sh

cat << EOF > CMakeLists.txt
add_library( modules STATIC `echo *.cc */*.cc` )
add_dependencies( modules pch )
EOF
