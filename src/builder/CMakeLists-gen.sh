#!/bin/sh

cat << EOF > CMakeLists.txt
add_library( builder STATIC
  `echo *.{hh,cc} | sed 's| |\n  |g'` )
use_pch( builder pch )
target_link_libraries( builder \${FREEIMAGE_LIBRARY} )
if( OZ_NONFREE )
  target_link_libraries( builder \${SQUISH_LIBRARY} )
endif()
EOF
