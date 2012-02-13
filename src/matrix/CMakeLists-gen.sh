#!/bin/sh

cat << EOF > CMakeLists.txt
add_library( matrix STATIC `echo *.cc` )
add_dependencies( matrix pch )
EOF
