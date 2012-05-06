#!/bin/sh

cat << EOF > CMakeLists.txt
add_executable( ozbuild `echo *.cc` )
add_dependencies( ozbuild pch )
target_link_libraries( ozbuild client modules nirvana matrix common build_info oz \${libs_tools} ozmain )

install( TARGETS ozbuild COMPONENT tools RUNTIME DESTINATION bin )
EOF
