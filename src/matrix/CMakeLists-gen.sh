#!/bin/sh

cat << EOF > CMakeLists.txt
add_library( matrix STATIC
`LC_COLLATE=C ls *.{hpp,cpp} | xargs printf '  %s\n'`
)

add_dependencies( matrix pch )
EOF
