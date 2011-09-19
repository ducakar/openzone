#!/bin/sh

cat << EOF > CMakeLists.txt
add_library( matrix STATIC
`LC_COLLATE=C ls *.cpp | xargs printf '  %s\n'`
`LC_COLLATE=C ls modules/*.cpp | xargs printf '  %s\n'`
)

add_dependencies( matrix pch )
EOF
