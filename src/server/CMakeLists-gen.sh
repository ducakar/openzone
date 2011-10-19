#!/bin/sh

cat << EOF > CMakeLists.txt
add_library( server STATIC
`LC_COLLATE=C ls *.cpp | xargs printf '  %s\n'`
)
add_dependencies( server pch )
EOF
