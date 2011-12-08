#!/bin/sh

cat << EOF > CMakeLists.txt
add_library( nirvana STATIC
`LC_COLLATE=C ls *.cc | xargs printf '  %s\n'`
)
add_dependencies( nirvana pch )
EOF
