#!/bin/sh

cat << EOF > CMakeLists.txt
add_library( common STATIC `echo *.cc` )
EOF
