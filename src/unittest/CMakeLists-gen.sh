#!/bin/sh

cat << EOF > CMakeLists.txt
add_executable( unittest
  `echo *.cc | sed 's/ /\n  /g'` )
target_link_libraries( unittest oz \${libs_oz} )

if( NACL )

  add_custom_command( OUTPUT unittest.nmf unittest.html
    DEPENDS "\${CMAKE_SOURCE_DIR}/etc/nacl/unittest.nmf" "\${CMAKE_SOURCE_DIR}/etc/nacl/unittest.html"
    COMMAND "\${CMAKE_COMMAND}" -E copy "\${CMAKE_SOURCE_DIR}/etc/nacl/unittest.nmf" unittest.nmf
    COMMAND "\${CMAKE_COMMAND}" -E copy "\${CMAKE_SOURCE_DIR}/etc/nacl/unittest.html" unittest.html )
  add_custom_target( unittest_files DEPENDS unittest.nmf unittest.html )

  add_dependencies( unittest unittest_files )
  set_target_properties( unittest PROPERTIES OUTPUT_NAME "unittest.\${CMAKE_SYSTEM_PROCESSOR}.nexe" )

endif()
EOF
