#!/bin/sh

cat << EOF > CMakeLists.txt
add_library( nirvana STATIC
  `echo *.{hh,cc} | sed 's/ /\n  /g'` )
use_pch( nirvana pch )
EOF
