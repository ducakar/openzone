#!/bin/sh

cat << EOF > CMakeLists.txt
add_library( oz STATIC
`LC_COLLATE=C ls *.cpp | xargs printf '  %s\n'`
)

if( OZ_INSTALL_LIBOZ )
  install( FILES
    \${CMAKE_BINARY_DIR}/src/oz/ozconfig.hpp
`LC_COLLATE=C ls *.{hpp,cpp} | xargs printf '    %s\n'`
    DESTINATION include/oz
    COMPONENT liboz )
  install( TARGETS oz DESTINATION lib COMPONENT liboz )
endif()
EOF
