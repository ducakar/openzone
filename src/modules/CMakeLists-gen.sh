#!/bin/sh

cat << EOF > CMakeLists.txt
add_library( modules STATIC
`LC_COLLATE=C ls *.cc | xargs printf '  %s\n'`
`LC_COLLATE=C ls */*.cc | xargs printf '  %s\n'`
)
add_dependencies( modules pch )
EOF
