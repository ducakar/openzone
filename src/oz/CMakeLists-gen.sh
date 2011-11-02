#!/bin/sh

cat << EOF > CMakeLists.txt
add_library( oz STATIC
`LC_COLLATE=C ls *.cpp | xargs printf '  %s\n'`
)

if( OZ_INSTALL_LIBOZ )
  install( FILES \${CMAKE_BINARY_DIR}/src/oz/ozconfig.hpp DESTINATION include/oz )
  install( DIRECTORY . DESTINATION include/oz FILES_MATCHING PATTERN "*.hpp" )
  install( TARGETS oz DESTINATION lib )
endif()
EOF
