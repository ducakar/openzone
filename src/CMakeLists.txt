configure_file( oz/ozconfig.hpp.in oz/ozconfig.hpp )
configure_file( configuration.hpp.in configuration.hpp )
configure_file( Build.cpp.in Build.cpp )

# required to include configuration headers when we use a separate build directory
# the second entry must be included before '.', so precompiled header is used instead the actual one
include_directories( ${CMAKE_CURRENT_BINARY_DIR}/oz )
include_directories( ${CMAKE_CURRENT_BINARY_DIR} )

include_directories( . )

add_subdirectory( oz )

add_pch( pch stable.hpp DEPENDS oz ${CMAKE_CURRENT_BINARY_DIR}/configuration.hpp )

add_library( build STATIC ${CMAKE_CURRENT_BINARY_DIR}/Build.cpp )
# add_custom_target( build_rebuild
#   COMMAND ${CMAKE_COMMAND} -E touch_nocreate ${CMAKE_CURRENT_BINARY_DIR}/Build.cpp )
# add_dependencies( build build_rebuild )

add_subdirectory( matrix )
add_subdirectory( nirvana )
add_subdirectory( arch )

if( OZ_BUILD_CLIENT )
  add_subdirectory( client )
endif()

if( OZ_BUILD_SERVER )
  add_subdirectory( server )
endif()

add_subdirectory( tools )

if( OZ_BUILD_TESTS )
  add_subdirectory( tests )
endif()
