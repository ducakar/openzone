#!/bin/sh

cat << EOF > CMakeLists.txt
add_library( matrix STATIC `LC_COLLATE=C echo *.cc` )
add_dependencies( matrix pch )
EOF
