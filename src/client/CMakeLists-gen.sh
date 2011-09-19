#!/bin/sh

cat << EOF > CMakeLists.txt
add_library( client STATIC
`LC_COLLATE=C ls *.cpp | xargs printf '  %s\n'`
`LC_COLLATE=C ls ui/*.cpp | xargs printf '  %s\n'`
)

add_dependencies( client nirvana matrix pch )
EOF
