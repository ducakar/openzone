include_directories( . )
configure_file( configuration.hpp.in configuration.hpp )

add_subdirectory( oz )

# precompiled header
if( UNIX )
	add_custom_target( pch DEPENDS stable.hpp.gch )
	add_custom_command( OUTPUT stable.hpp.gch
						DEPENDS oz stable.hpp
						COMMAND rm -f stable.hpp.gch && ${CMAKE_CXX_COMPILER} `echo ${flags}` -I. stable.hpp )
else( UNIX )
	add_custom_target( pch )
endif( UNIX )

add_subdirectory( matrix )
add_subdirectory( nirvana )
add_subdirectory( ui )
add_subdirectory( client )
#add_subdirectory( server )
add_subdirectory( tools )
add_subdirectory( tests )
