#!/bin/sh

cat << EOF > CMakeLists.txt
add_library( client STATIC
`LC_COLLATE=C ls *.{hpp,cpp} | xargs printf '  %s\n'`
`LC_COLLATE=C ls ui/*.{hpp,cpp} | xargs printf '  %s\n'`
)

add_dependencies( client arch nirvana matrix pch )
EOF
