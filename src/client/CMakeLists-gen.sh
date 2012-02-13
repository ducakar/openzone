#!/bin/sh

cat << EOF > CMakeLists.txt
add_library( client STATIC `echo *.cc ui/*.cc | sed 's/openzone\.cc//'` )
add_dependencies( client pch )

add_executable( openzone openzone.cc )
add_dependencies( openzone pch )
target_link_libraries( openzone client modules nirvana matrix common build_info oz \${libs_client} )

install( TARGETS openzone COMPONENT client RUNTIME DESTINATION bin )
EOF
