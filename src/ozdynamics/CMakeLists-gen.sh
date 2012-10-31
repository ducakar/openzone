#!/bin/sh

cat << EOF > CMakeLists.txt
set( headers
  `echo *.hh */*.hh | sed 's| |\n  |g'` )
add_library( ozdynamics
  `echo *.hh */*.{hh,cc} | sed 's| |\n  |g'` )
set_target_properties( ozdynamics PROPERTIES
                       PUBLIC_HEADER "\${headers}"
                       VERSION \${OZ_VERSION}
                       SOVERSION 0 )
target_link_libraries( ozdynamics oz )

install( TARGETS ozdynamics
         RUNTIME DESTINATION bin
         LIBRARY ARCHIVE DESTINATION lib
         PUBLIC_HEADER DESTINATION include/ozdynamics )
EOF
