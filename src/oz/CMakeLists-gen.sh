#!/bin/sh

cat << EOF > CMakeLists.txt
configure_file( liboz.pc.in liboz.pc )
configure_file( ozconfig.hh.in ozconfig.hh )

add_library( oz SHARED `LC_COLLATE=C  echo *.hh *.cc` )
set_target_properties( oz PROPERTIES PUBLIC_HEADER "`LC_COLLATE=C echo *.hh | tr ' ' ';'`" )
set_target_properties( oz PROPERTIES SOVERSION \${OZ_SOVERSION} )
target_link_libraries( oz LINK_PRIVATE \${libs_oz} )

install( FILES \${CMAKE_CURRENT_BINARY_DIR}/liboz.pc COMPONENT liboz
         DESTINATION lib\${OZ_LIB_SUFFIX}/pkgconfig )
install( TARGETS oz COMPONENT liboz
         RUNTIME DESTINATION bin
         LIBRARY DESTINATION lib\${OZ_LIB_SUFFIX}
         ARCHIVE DESTINATION lib\${OZ_LIB_SUFFIX}
         PUBLIC_HEADER DESTINATION include/oz )
EOF
