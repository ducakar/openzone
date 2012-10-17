#!/bin/sh

cat << EOF > CMakeLists.txt
configure_file( liboz.pc.in liboz.pc )
configure_file( ozconfig.hh.in ozconfig.hh )

include_directories( \${CMAKE_CURRENT_BINARY_DIR} )

set( headers
  \${CMAKE_CURRENT_BINARY_DIR}/ozconfig.hh
  `echo *.hh | sed 's| |\n  |g'` )
add_library( oz
  `echo *.{hh,cc} | sed 's| |\n  |g'` )
set_target_properties( oz PROPERTIES
                       PUBLIC_HEADER "\${headers}"
                       RESOURCE \${CMAKE_CURRENT_BINARY_DIR}/liboz.pc
                       VERSION \${OZ_VERSION}
                       SOVERSION 0 )
target_link_libraries( oz \${libs_oz} )

install( TARGETS oz
         RUNTIME DESTINATION bin
         LIBRARY DESTINATION lib
         ARCHIVE DESTINATION lib
         PUBLIC_HEADER DESTINATION include/oz
         RESOURCE DESTINATION lib/pkgconfig )
EOF
