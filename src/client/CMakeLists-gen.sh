#!/bin/sh

cat << EOF > CMakeLists.txt
add_library( client STATIC
`LC_COLLATE=C ls *.cc | xargs printf '  %s\n'`
`LC_COLLATE=C ls ui/*.cc | xargs printf '  %s\n'`
)
add_dependencies( client pch )

add_executable( openzone openzone/openzone.cc )
add_dependencies( openzone pch )
target_link_libraries( openzone client modules nirvana matrix common build_info oz \${libs_client} )

if( OZ_INSTALL_CLIENT )
  install( TARGETS openzone DESTINATION bin )
endif()
EOF
