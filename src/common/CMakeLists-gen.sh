#!/bin/sh

cat << EOF > CMakeLists.txt
add_library( common STATIC
`LC_COLLATE=C ls *.cc | xargs printf '  %s\n'`
)
EOF
