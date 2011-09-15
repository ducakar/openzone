#!/bin/sh

cat << EOF > CMakeLists.txt
add_library( arch STATIC
`LC_COLLATE=C ls *.{hpp,cpp} | xargs printf '  %s\n'`
)

add_dependencies( arch nirvana matrix pch )
EOF
