#!/bin/sh

cat << EOF > CMakeLists.txt
add_library( modules STATIC `LC_COLLATE=C echo *.cc */*.cc` )
add_dependencies( modules pch )
EOF
