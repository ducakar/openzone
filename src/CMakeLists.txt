include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )
include_directories( ${CMAKE_CURRENT_BINARY_DIR} )

add_pch( pch pch.hh pch.cc )

add_subdirectory( ozCore )
add_subdirectory( ozDynamics )
add_subdirectory( ozEngine )
add_subdirectory( ozFactory )
add_subdirectory( unittest )

add_subdirectory( common )
add_subdirectory( matrix )
add_subdirectory( nirvana )
add_subdirectory( client )
add_subdirectory( tools )
add_subdirectory( builder )
add_subdirectory( tests )
