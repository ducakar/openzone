#!/bin/sh

cat << EOF > CMakeLists.txt
add_library( client STATIC
`LC_COLLATE=C ls *.cpp | xargs printf '  %s\n'`
`LC_COLLATE=C ls ui/*.cpp | xargs printf '  %s\n'`
`LC_COLLATE=C ls modules/*.cpp | xargs printf '  %s\n'`
)
add_dependencies( client nirvana matrix pch )

add_executable( openzone openzone/openzone.cpp )
add_dependencies( openzone client nirvana matrix build pch )
target_link_libraries( openzone build client nirvana matrix build oz \${libs} )

if( OZ_INSTALL_OPENZONE )
  install( TARGETS openzone DESTINATION bin )
endif()
EOF
