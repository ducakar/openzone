#!/bin/sh

cat << EOF > CMakeLists.txt
add_library( oz STATIC
`LC_COLLATE=C ls *.cc | xargs printf '  %s\n'`
)

if( OZ_INSTALL_LIBOZ )
  add_library( liboz SHARED
  `LC_COLLATE=C ls *.cc | xargs printf '  %s\n'`
  )
  target_link_libraries( liboz \${libs} )

  install( FILES \${CMAKE_CURRENT_BINARY_DIR}/ozconfig.hh DESTINATION include/oz COMPONENT liboz )
  install( DIRECTORY . DESTINATION include/oz COMPONENT liboz FILES_MATCHING PATTERN "*.hh" )
  install( TARGETS liboz DESTINATION lib COMPONENT liboz )
  install( FILES \${PROJECT_SOURCE_DIR}/etc/liboz/liboz.pc DESTINATION lib/pkgconfig COMPONENT liboz )
  install( FILES \${PROJECT_SOURCE_DIR}/etc/liboz/COPYING DESTINATION doc/liboz COMPONENT liboz )
endif()
EOF
