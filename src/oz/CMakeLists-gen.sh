#!/bin/sh

cat << EOF > CMakeLists.txt
configure_file( liboz.pc.in liboz.pc )
configure_file( ozconfig.hh.in ozconfig.hh )

add_library( oz `echo *.cc` )
set_target_properties( oz PROPERTIES PUBLIC_HEADER "`echo *.hh | tr ' ' ';'`" )
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
