#!/bin/sh

cat << EOF > CMakeLists.txt
add_library( nirvana STATIC
`LC_COLLATE=C ls *.cpp | xargs printf '  %s\n'`
)

add_dependencies( nirvana matrix pch )
EOF
