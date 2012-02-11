#!/bin/sh

cat << EOF > CMakeLists.txt
add_library( nirvana STATIC `LC_COLLATE=C echo *.cc` )
add_dependencies( nirvana pch )
EOF
