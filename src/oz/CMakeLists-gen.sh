#!/bin/sh

cat << EOF > CMakeLists.txt
configure_file( ozconfig.hh.in ozconfig.hh )

add_library( oz SHARED
`LC_COLLATE=C ls *.cc | xargs printf '  %s\n'`
)
target_link_libraries( oz \${libs_oz} )
set_target_properties( oz PROPERTIES SOVERSION \${OZ_SOVERSION} )

if( OZ_INSTALL_LIBOZ )
  install( FILES \${CMAKE_CURRENT_BINARY_DIR}/ozconfig.hh DESTINATION include/oz )
  install( DIRECTORY . DESTINATION include/oz FILES_MATCHING PATTERN "*.hh" )
  install( TARGETS oz DESTINATION lib\${OZ_LIB_SUFFIX} )
endif()
EOF
