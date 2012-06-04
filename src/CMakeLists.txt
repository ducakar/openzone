configure_file( configuration.hh.in configuration.hh )
configure_file( BuildInfo.cc.in BuildInfo.cc )

# Required to include configuration headers when we use a separate build directory. The second entry
# must be included before '.', so directory with precompiled header is before directory with only
# source header in the search path.
include_directories( ${CMAKE_BINARY_DIR}/src/oz )
include_directories( ${CMAKE_BINARY_DIR}/src )
include_directories( . )

add_subdirectory( oz )

add_subdirectory( common )

add_pch( pch stable.hh stable.cc )

add_subdirectory( matrix )
add_subdirectory( nirvana )
add_subdirectory( modules )
add_subdirectory( client )

if( NOT NACL AND NOT ANDROID )
  add_subdirectory( build )
  add_subdirectory( tools )
endif()

add_subdirectory( tests )

add_library( build_info STATIC ${CMAKE_BINARY_DIR}/src/BuildInfo.cc )
