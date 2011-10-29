#!/bin/sh

cat << EOF > CMakeLists.txt
add_library( common STATIC
`LC_COLLATE=C ls *.cpp | xargs printf '  %s\n'`
)
EOF
