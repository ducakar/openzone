#!/bin/sh

cat << EOF > CMakeLists.txt
add_executable( ozBuild
  `echo *.cc | sed 's/ /\n  /g'` )
add_dependencies( ozBuild pch )
target_link_libraries( ozBuild client modules nirvana matrix common build_info oz \${libs_tools} )

if( OZ_STANDALONE )
  install( TARGETS ozBuild RUNTIME DESTINATION bin/\${OZ_SYSTEM_NAME} )
else()
  install( TARGETS ozBuild RUNTIME DESTINATION bin )
endif()
EOF