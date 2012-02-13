#!/bin/sh

cat << EOF > CMakeLists.txt
add_library( nirvana STATIC `echo *.cc` )
add_dependencies( nirvana pch )
EOF
