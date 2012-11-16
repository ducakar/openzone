include_directories( . )
include_directories( ${CMAKE_CURRENT_BINARY_DIR} )

add_subdirectory( ozCore )
add_subdirectory( ozDynamics )
add_subdirectory( unittest )

add_pch( pch stable.hh stable.cc )
add_subdirectory( common )

add_subdirectory( matrix )
add_subdirectory( nirvana )
add_subdirectory( modules )
add_subdirectory( client )
add_subdirectory( tools )

if( NOT ANDROID AND NOT NACL )
  add_subdirectory( builder )
  add_subdirectory( txc_dxtn )
endif()

add_subdirectory( tests )
