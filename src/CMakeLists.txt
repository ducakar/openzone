# precompiled header
if( CMAKE_BUILD_TYPE STREQUAL Debug )
    set( flags "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG}" )
elseif( CMAKE_BUILD_TYPE STREQUAL Release )
    set( flags "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELEASE}" )
elseif( CMAKE_BUILD_TYPE STREQUAL RelWithDebInfo )
    set( flags "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELWITHDEBINFO}" )
else()
    set( flags "${CMAKE_CXX_FLAGS}" )
endif()

add_custom_target( pch DEPENDS stable.h.gch )
add_custom_command( OUTPUT stable.h.gch
                    DEPENDS base stable.h
                    COMMAND rm -f stable.h.gch &&
                            ${CMAKE_CXX_COMPILER} `echo ${flags}` -I. stable.h )

configure_file( config.h.in config.h )

include_directories( . )

add_subdirectory( base )
add_subdirectory( matrix )
add_subdirectory( nirvana )
add_subdirectory( ui )
add_subdirectory( client )
#add_subdirectory( server )
add_subdirectory( tools )
add_subdirectory( tests )
