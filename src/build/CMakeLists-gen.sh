#!/bin/sh

cat << EOF > CMakeLists.txt
add_executable( ozBuild
`LC_COLLATE=C ls *.cc | xargs printf '  %s\n'`
`LC_COLLATE=C ls modules/*.cc | xargs printf '  %s\n'`
)
add_dependencies( ozBuild pch )
target_link_libraries( ozBuild client nirvana matrix common build_info oz \${libs_tools} )

if( OZINST_TOOLS )
  install( TARGETS ozBuild DESTINATION bin )
endif()
EOF
