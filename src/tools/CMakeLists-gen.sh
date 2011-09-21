#!/bin/sh

cat << EOF > CMakeLists.txt
add_definitions( -DOZ_TOOLS )

add_library( matrix-tools STATIC
`LC_COLLATE=C ls ../matrix/*.cpp | xargs printf '  %s\n'`
`LC_COLLATE=C ls ../matrix/modules/*.cpp | xargs printf '  %s\n'`
)
add_dependencies( matrix-tools pch )

add_library( client-tools STATIC
`LC_COLLATE=C ls ../client/*.cpp | xargs printf '  %s\n'`
`LC_COLLATE=C ls ../client/ui/*.cpp | xargs printf '  %s\n'`
)
add_dependencies( client-tools nirvana matrix-tools pch )

add_executable( ozPrebuild ozPrebuild.cpp )
add_dependencies( ozPrebuild client-tools nirvana matrix-tools build pch )
target_link_libraries( ozPrebuild client-tools nirvana matrix-tools build oz \${libs_tools} )

if( OZ_INSTALL_TOOLS )
  install( TARGETS ozPrebuild DESTINATION bin )
endif()
EOF
