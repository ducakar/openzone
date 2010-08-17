configure_file( oz/ozconfig.hpp.in oz/ozconfig.hpp )
configure_file( configuration.hpp.in configuration.hpp )

# needed to include configuration headers when we use a separate build directory
include_directories( ${CMAKE_CURRENT_BINARY_DIR}/oz )
include_directories( ${CMAKE_CURRENT_BINARY_DIR} )

include_directories( . )

add_subdirectory( oz )

add_pch( pch stable.hpp DEPENDS oz ${CMAKE_CURRENT_BINARY_DIR}/configuration.hpp )

add_subdirectory( matrix )
add_subdirectory( nirvana )
add_subdirectory( ui )
add_subdirectory( client )
#add_subdirectory( server )
add_subdirectory( tools )
add_subdirectory( tests )
