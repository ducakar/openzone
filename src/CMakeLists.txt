configure_file( oz/ozconfig.hh.in oz/ozconfig.hh )
configure_file( configuration.hh.in configuration.hh )
configure_file( BuildInfo.cc.in BuildInfo.cc )

# Required to include configuration headers when we use a separate build directory. The second entry
# must be included before '.', so the precompiled header is used instead the actual one.
include_directories( ${CMAKE_BINARY_DIR}/src/oz )
include_directories( ${CMAKE_BINARY_DIR}/src )
include_directories( . )

add_subdirectory( oz )

add_library( build_info STATIC ${CMAKE_BINARY_DIR}/src/BuildInfo.cc )

add_pch( pch stable.hh stable.cc )

add_subdirectory( common )
add_subdirectory( matrix )
add_subdirectory( nirvana )
add_subdirectory( modules )
add_subdirectory( client )
add_subdirectory( build )
add_subdirectory( tools )
add_subdirectory( tests )
