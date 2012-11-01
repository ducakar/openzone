#!/bin/sh

cat << EOF > CMakeLists.txt
add_library( ozdynamics
  `echo *.hh */*.{hh,cc} | sed 's| |\n  |g'` )
set_target_properties( ozdynamics PROPERTIES VERSION \${OZ_VERSION} SOVERSION 0 )
target_link_libraries( ozdynamics oz )

install( TARGETS ozdynamics
         RUNTIME DESTINATION bin COMPONENT libozdynmics
         LIBRARY DESTINATION lib COMPONENT libozdynmics
         ARCHIVE DESTINATION lib COMPONENT libozdynmics )
install( DIRECTORY \${CMAKE_CURRENT_SOURCE_DIR} DESTINATION include COMPONENT libozdynamics
         FILES_MATCHING PATTERN *.hh )
install( FILES COPYING DESTINATION share/doc/libozdynamics-\${OZ_VERSION} COMPONENT libozdynamics )
EOF
