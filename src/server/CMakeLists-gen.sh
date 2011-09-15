#!/bin/sh

cat << EOF > CMakeLists.txt
add_library( server STATIC
`LC_COLLATE=C ls *.{hpp,cpp} | xargs printf '  %s\n'`
)

add_dependencies( server arch nirvana matrix pch )
EOF
