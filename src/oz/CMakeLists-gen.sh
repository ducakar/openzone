#!/bin/sh

cat << EOF > CMakeLists.txt
add_library( oz SHARED
`LC_COLLATE=C ls *.cc | xargs printf '  %s\n'`
)
target_link_libraries( oz \${libs_oz} )
set_target_properties( oz PROPERTIES SOVERSION \${OZ_SOVERSION} )

if( OZINST_LIBOZ )
  install( FILES \${CMAKE_CURRENT_BINARY_DIR}/ozconfig.hh DESTINATION include/oz )
  install( DIRECTORY . DESTINATION include/oz FILES_MATCHING PATTERN "*.hh" )
  install( TARGETS oz DESTINATION lib )
  install( FILES \${PROJECT_SOURCE_DIR}/etc/liboz/liboz.pc DESTINATION lib/pkgconfig )
  install( FILES \${PROJECT_SOURCE_DIR}/etc/liboz/COPYING DESTINATION share/doc/liboz )
endif()
EOF
