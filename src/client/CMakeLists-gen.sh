#!/bin/sh

cat << EOF > CMakeLists.txt
add_library( client STATIC
  `echo *.cc ui/*.cc | sed 's/ /\n  /g' | grep -v 'openzone\.cc'` )
add_dependencies( client pch )

add_executable( openzone openzone.cc )
add_dependencies( openzone pch )
target_link_libraries( openzone client modules nirvana matrix common build_info oz \${libs_client} )

if( NACL )
  add_custom_command( OUTPUT openzone.nmf openzone.sl.html
    DEPENDS "\${CMAKE_SOURCE_DIR}/etc/nacl/openzone.nmf" "\${CMAKE_SOURCE_DIR}/etc/nacl/openzone.sl.html"
    COMMAND \${CMAKE_COMMAND} -E copy "\${CMAKE_SOURCE_DIR}/etc/nacl/openzone.nmf" openzone.nmf
    COMMAND \${CMAKE_COMMAND} -E copy "\${CMAKE_SOURCE_DIR}/etc/nacl/openzone.sl.html" openzone.sl.html )
  add_custom_target( openzone_files DEPENDS openzone.nmf openzone.sl.html )

  add_dependencies( openzone openzone_files )
  set_target_properties( openzone PROPERTIES OUTPUT_NAME "openzone.\${CMAKE_SYSTEM_PROCESSOR}.nexe" )
endif()

install( TARGETS openzone RUNTIME DESTINATION bin )
EOF
