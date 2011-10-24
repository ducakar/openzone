configure_file( oz/ozconfig.hpp.in oz/ozconfig.hpp )
configure_file( configuration.hpp.in configuration.hpp )
configure_file( Build.cpp.in Build.cpp )

# Required to include configuration headers when we use a separate build directory. The second entry
# must be included before '.', so the precompiled header is used instead the actual one.
include_directories( ${CMAKE_BINARY_DIR}/src/oz )
include_directories( ${CMAKE_BINARY_DIR}/src )

include_directories( . )

add_subdirectory( oz )

add_pch( pch stable.hpp DEPENDS oz ${CMAKE_BINARY_DIR}/src/configuration.hpp )

add_library( build STATIC ${CMAKE_BINARY_DIR}/src/Build.cpp )

add_subdirectory( matrix )
add_subdirectory( nirvana )
add_subdirectory( client )
# add_subdirectory( server )
add_subdirectory( tools )
add_subdirectory( tests )
