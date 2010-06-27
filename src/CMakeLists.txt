configure_file( configuration.hh.in configuration.hh )

include_directories( . )

add_subdirectory( oz )

# precompiled header
if( UNIX )
	add_custom_target( pch DEPENDS stable.hh.gch )
	add_custom_command( OUTPUT stable.hh.gch
		DEPENDS oz stable.hh
		COMMAND rm -f stable.hh.gch && ${CMAKE_CXX_COMPILER} `echo ${flags}` -I. stable.hh )
else( UNIX )
	add_custom_target( pch )
endif( UNIX )

add_subdirectory( matrix )
add_subdirectory( nirvana )
add_subdirectory( ui )
add_subdirectory( client )
#add_subdirectory( server )
#add_subdirectory( tools )
#add_subdirectory( tests )
