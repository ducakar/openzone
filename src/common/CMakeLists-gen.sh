#!/bin/sh

cat << EOF > CMakeLists.txt
add_library( common STATIC `LC_COLLATE=C echo *.cc` )
EOF
