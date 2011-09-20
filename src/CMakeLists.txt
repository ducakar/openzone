configure_file( oz/ozconfig.hpp.in oz/ozconfig.hpp )
configure_file( configuration.hpp.in configuration.hpp )
configure_file( Build.cpp.in Build.cpp )

# required to include configuration headers when we use a separate build directory
# the second entry must be included before '.', so precompiled header is used instead the actual one
include_directories( ${CMAKE_BINARY_DIR}/src/oz )
include_directories( ${CMAKE_BINARY_DIR}/src )

include_directories( . )

add_subdirectory( oz )

add_pch( pch stable.hpp DEPENDS oz ${CMAKE_BINARY_DIR}/src/configuration.hpp )

add_library( build STATIC ${CMAKE_BINARY_DIR}/src/Build.cpp )
# add_custom_target( build_rebuild
#   COMMAND ${CMAKE_COMMAND} -E touch_nocreate ${CMAKE_BINARY_DIR}/src/Build.cpp )
# add_dependencies( build build_rebuild )

add_subdirectory( matrix )
add_subdirectory( nirvana )
add_subdirectory( client )
# add_subdirectory( server )
add_subdirectory( tools )
add_subdirectory( tests )
add_subdirectory( syncheck )
