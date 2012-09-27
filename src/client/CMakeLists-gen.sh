#!/bin/sh

cat << EOF > CMakeLists.txt
add_library( client STATIC
  `echo *.{hh,cc} ui/*.{hh,cc} | sed 's/ /\n  /g' | grep -v 'openzone\.cc'` )
add_dependencies( client pch )

if( OZ_JNI )
  add_library( openzone SHARED openzone.cc )
else()
  add_executable( openzone openzone.cc )
endif()
add_dependencies( openzone pch )
target_link_libraries( openzone client modules nirvana matrix common build_info oz \${libs_client} )

if( NACL )
  set_target_properties( openzone PROPERTIES OUTPUT_NAME "openzone.\${CMAKE_SYSTEM_PROCESSOR}.nexe" )
endif()

if( OZ_STANDALONE )
  install( TARGETS openzone RUNTIME DESTINATION bin/\${OZ_SYSTEM_NAME} LIBRARY )
else()
  install( TARGETS openzone RUNTIME DESTINATION bin LIBRARY DESTINATION lib )
endif()
EOF
