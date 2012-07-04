#!/bin/sh

cat << EOF > CMakeLists.txt
configure_file( liboz.pc.in liboz.pc )
configure_file( ozconfig.hh.in ozconfig.hh )

set( ozHeaders
  `echo *.hh | sed 's/ /\n  /g'` )
add_library( oz
  `echo *.cc | sed 's/ /\n  /g'` )
set_target_properties( oz PROPERTIES PUBLIC_HEADER "\${ozHeaders}" )
set_target_properties( oz PROPERTIES VERSION "\${OZ_VERSION}" SOVERSION "0" )
target_link_libraries( oz \${libs_oz} )

install( FILES "\${CMAKE_CURRENT_BINARY_DIR}/liboz.pc" DESTINATION lib\${libSuffix}/pkgconfig )
install( FILES "\${CMAKE_CURRENT_BINARY_DIR}/ozconfig.hh" DESTINATION include/oz )
install( TARGETS oz
         RUNTIME DESTINATION bin
         LIBRARY DESTINATION lib\${libSuffix}
         ARCHIVE DESTINATION lib\${libSuffix}
         PUBLIC_HEADER DESTINATION include/oz )
EOF
