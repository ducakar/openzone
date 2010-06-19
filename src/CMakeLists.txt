include_directories( . )
configure_file( configuration.h.in configuration.h )

add_subdirectory( oz )

# precompiled header
if( UNIX )
    add_custom_target( pch DEPENDS stable.h.gch )
    add_custom_command( OUTPUT stable.h.gch
                        DEPENDS oz stable.h
                        COMMAND rm -f stable.h.gch && ${CMAKE_CXX_COMPILER} `echo ${flags}` -I. stable.h )
endif( UNIX )

add_subdirectory( matrix )
add_subdirectory( nirvana )
add_subdirectory( ui )
add_subdirectory( client )
#add_subdirectory( server )
add_subdirectory( tools )
add_subdirectory( tests )
