#!/bin/sh

cat << EOF > CMakeLists.txt
configure_file( libozdyn.pc.in libozdyn.pc )

set( headers
  `echo *.hh */*.hh | sed 's/ /\n  /g'` )
add_library( ozdyn
  `echo *.hh */*.{hh,cc} | sed 's/ /\n  /g'` )

set_target_properties( ozdyn PROPERTIES
  PUBLIC_HEADER "\${headers}"
  RESOURCE "\${CMAKE_CURRENT_BINARY_DIR}/libozdyn.pc"
  VERSION "\${OZ_VERSION}"
  SOVERSION "0" )
target_link_libraries( ozdyn oz )

install( TARGETS ozdyn
         RUNTIME DESTINATION bin
         LIBRARY DESTINATION lib
         ARCHIVE DESTINATION lib
         PUBLIC_HEADER DESTINATION include/ozdyn
         RESOURCE DESTINATION lib/pkgconfig )
EOF
