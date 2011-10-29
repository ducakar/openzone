#!/bin/sh

cat << EOF > CMakeLists.txt
add_executable( ozPrebuild
`LC_COLLATE=C ls *.cpp | xargs printf '  %s\n'`
)
add_dependencies( ozPrebuild pch )
target_link_libraries( ozPrebuild client nirvana matrix common build oz \${libs_tools} )

if( OZ_INSTALL_TOOLS )
  install( TARGETS ozPrebuild DESTINATION bin )
endif()
EOF
