configure_file( oz/ozconfig.hpp.in oz/ozconfig.hpp )
configure_file( configuration.hpp.in configuration.hpp )

# required to include configuration headers when we use a separate build directory
# the second entry must be included before '.', so precompiled header is used instead the actual one
include_directories( ${CMAKE_CURRENT_BINARY_DIR}/oz )
include_directories( ${CMAKE_CURRENT_BINARY_DIR} )

include_directories( . )

add_subdirectory( oz )

add_pch( pch stable.hpp DEPENDS oz ${CMAKE_CURRENT_BINARY_DIR}/configuration.hpp )

add_subdirectory( matrix )
add_subdirectory( nirvana )
add_subdirectory( architect )

if( OZ_BUILD_CLIENT )
  add_subdirectory( ui )
  add_subdirectory( client )
endif( OZ_BUILD_CLIENT )

if( OZ_BUILD_SERVER )
  add_subdirectory( server )
endif( OZ_BUILD_SERVER )

add_subdirectory( tools )

if( OZ_BUILD_TESTS )
  add_subdirectory( tests )
endif( OZ_BUILD_TESTS )

