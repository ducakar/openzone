cmake_minimum_required( VERSION 2.8 )

project( openzone C CXX )

set( CPACK_PACKAGE_VERSION_MAJOR 0 )
set( CPACK_PACKAGE_VERSION_MINOR 0 )
set( CPACK_PACKAGE_VERSION_PATCH 0 )

option( OZ_ALLOC_STATISTICS "Use overloads for default new and delete that make heap usage statistics" ON )
option( OZ_POOL_ALLOC "Enable PoolAlloc and new/delete overloads that use it. Otherwise a dummy class that calls malloc/free is used and no overloads occur." ON )
option( OZ_GNU_MATH "Use GCC math built-ins (no need to include <cmath> and faster calls in debug mode)" ON )
option( OZ_VERBOSE_CONFIG "Print warnings about missing or unused variables in configuration files" ON )
option( OZ_XML_CONFIG "Support for XML configuration files (Linux only)" OFF )
option( OZ_CROSS_MINGW "If MinGW is not used natively on Windows, but on Unix for cross-compiling." OFF )
option( OZ_BUILD "Optimise and dump resources where possible. Those can be used by a OZ_PREBUILT enabled build." OFF )
option( OZ_PREBUILT "If OpenZone optimised resources should be loaded instead of generic ones" OFF )
option( OZ_ENABLE_CYLINDER "Enable cylinder collision shape and physics" OFF )

include( CheckLibraryExists )
include( CPack )

macro( add_pch _targetName _inputHeader )
	 if( NOT MSVC )
		  # extract CMAKE_CXX_FLAGS and CMAKE_CXX_FLAGS_XXX for current build configuration XXX
		  string( TOUPPER "CMAKE_CXX_FLAGS_${CMAKE_BUILD_TYPE}" _build_type_flags_var )
		  set( _flags "${CMAKE_CXX_FLAGS} ${${_build_type_flags_var}}" )

		  # convert string of space separated flags into a list
		  separate_arguments( _flags )

		  # extract include directories set by include_directories command
		  get_directory_property( _includes INCLUDE_DIRECTORIES )
		  foreach( _inc_dir ${_includes} )
				list( APPEND _flags "-I${_inc_dir}" )
		  endforeach( _inc_dir ${_includes} )

		  # set build rules
		  add_custom_command( OUTPUT ${_inputHeader}.gch
				${ARGN} ${_inputHeader}
				COMMAND ${CMAKE_COMMAND} -E remove ${_inputHeader}.gch
				COMMAND cd
				COMMAND ${CMAKE_CXX_COMPILER} ${_flags} -o ${_inputHeader}.gch ${CMAKE_CURRENT_SOURCE_DIR}/${_inputHeader} )
		  add_custom_target( ${_targetName} DEPENDS ${_inputHeader}.gch )
	 else( NOT MSVC )
		  add_custom_target( ${_targetName} SOURCES ${_inputHeader} )
	 endif( NOT MSVC )
endmacro( add_pch )

if( NOT CMAKE_BUILD_TYPE )
	set( CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type" FORCE )
endif()

check_library_exists( m sincosf "" OZ_HAVE_SINCOSF )

if( OZ_XML_CONFIG )
	Find_Package( LibXml2 REQUIRED )
endif( OZ_XML_CONFIG )

Find_Package( SDL REQUIRED )
Find_Package( SDL_image )
Find_Package( SDL_net )
Find_Package( SDL_ttf )
Find_Package( Lua51 )
Find_Package( OpenGL )
Find_Package( OpenAL )

if( NOT ALUT_INCLUDE_DIR )
	set( ALUT_INCLUDE_DIR "ALUT_INCLUDE_DIR-NOTFOUND" CACHE PATH "" FORCE )
endif( NOT ALUT_INCLUDE_DIR )

if( NOT ALUT_LIBRARY )
	set( ALUT_LIBRARY "ALUT_LIBRARY-NOTFOUND" CACHE FILEPATH "" FORCE )
endif( NOT ALUT_LIBRARY )

if( NOT OGG_INCLUDE_DIR )
	set( OGG_INCLUDE_DIR "OGG_INCLUDE_DIR-NOTFOUND" CACHE PATH "" FORCE )
endif( NOT OGG_INCLUDE_DIR )

if( NOT OGG_LIBRARY )
	set( OGG_LIBRARY "OGG_LIBRARY-NOTFOUND" CACHE FILEPATH "" FORCE )
endif( NOT OGG_LIBRARY )

if( NOT VORBIS_INCLUDE_DIR )
	set( VORBIS_INCLUDE_DIR "VORBIS_INCLUDE_DIR-NOTFOUND" CACHE PATH "" FORCE )
endif( NOT VORBIS_INCLUDE_DIR )

if( NOT VORBIS_LIBRARY )
	set( VORBIS_LIBRARY "VORBIS_LIBRARY-NOTFOUND" CACHE FILEPATH "" FORCE )
endif( NOT VORBIS_LIBRARY )

if( NOT VORBISFILE_LIBRARY )
	set( VORBISFILE_LIBRARY "VORBISFILE_LIBRARY-NOTFOUND" CACHE FILEPATH "" FORCE )
endif( NOT VORBISFILE_LIBRARY )

if( MSVC AND NOT DIRENT_INCLUDE_DIR )
	set( DIRENT_INCLUDE_DIR "DIRENT_INCLUDE_DIR-NOTFOUND" CACHE PATH "" FORCE )
endif( MSVC AND NOT DIRENT_INCLUDE_DIR )

if( MSVC )

	set( OZ_MSVC ON )

	# compiler and linker flags
	set( warnings "/W3 /D _CRT_SECURE_NO_WARNINGS" )
	set( flags "/EHsc" )
	set( libs "" )
	set( arch_flags "/arch:SSE2" )

	# set platform-specific includes and library names
	include_directories( ${SDL_INCLUDE_DIR} )
	include_directories( ${SDLIMAGE_INCLUDE_DIR} )
	include_directories( ${SDLNET_INCLUDE_DIR} )
	include_directories( ${SDLTTF_INCLUDE_DIR} )
	include_directories( ${LUA_INCLUDE_DIR} )
	include_directories( ${OPENGL_INCLUDE_DIR} )
	include_directories( ${OPENAL_INCLUDE_DIR} )
	include_directories( ${ALUT_INCLUDE_DIR} )
	include_directories( ${OGG_INCLUDE_DIR} )
	include_directories( ${VORBIS_INCLUDE_DIR} )
	include_directories( ${DIRENT_INCLUDE_DIR} )

	set( libs "${libs};${SDL_LIBRARY}" )
	set( libs "${libs};${SDLMAIN_LIBRARY}" )
	set( libs "${libs};${SDLIMAGE_LIBRARY}" )
	set( libs "${libs};${SDLNET_LIBRARY}" )
	set( libs "${libs};${SDLTTF_LIBRARY}" )
	set( libs "${libs};${LUA_LIBRARY}" )
	set( libs "${libs};${OPENGL_gl_LIBRARY}" )
	set( libs "${libs};${OPENGL_glu_LIBRARY}" )
	set( libs "${libs};${OPENAL_LIBRARY}" )
	set( libs "${libs};${ALUT_LIBRARY}" )
	set( libs "${libs};${OGG_LIBRARY}" )
	set( libs "${libs};${VORBIS_LIBRARY}" )
	set( libs "${libs};${VORBISFILE_LIBRARY}" )

else( MSVC )

	if( WIN32 )
		set( OZ_MINGW ON )
	endif()

	# compiler and linker flags
	set( flags "-pedantic -std=c++0x -fmessage-length=0 -pipe" )
	set( warnings "-Wall -Wextra -Wold-style-cast -Wconversion -Wstrict-overflow" )
	set( warnings "${warnings} -Wlogical-op -Wredundant-decls -Wsign-promo -Wpacked" )
	set( warnings "${warnings} -Woverloaded-virtual -Wnon-virtual-dtor" )
	set( warnings "${warnings} -Wctor-dtor-privacy -Winvalid-pch -Wdisabled-optimization" )

	set( arch_flags "-mmmx -msse3 -mfpmath=sse" )

	include_directories( ${SDL_INCLUDE_DIR} )
	include_directories( ${SDLIMAGE_INCLUDE_DIR} )
	include_directories( ${SDLNET_INCLUDE_DIR} )
	include_directories( ${SDLTTF_INCLUDE_DIR} )
	include_directories( ${LUA_INCLUDE_DIR} )
	include_directories( ${OPENGL_INCLUDE_DIR} )
	include_directories( ${OPENAL_INCLUDE_DIR} )
	include_directories( ${ALUT_INCLUDE_DIR} )
	include_directories( ${OGG_INCLUDE_DIR} )
	include_directories( ${VORBIS_INCLUDE_DIR} )

	set( libs "${SDLMAIN_LIBRARY}" )
	set( libs "${libs};${SDL_LIBRARY}" )
	set( libs "${libs};${SDLIMAGE_LIBRARY}" )
	set( libs "${libs};${SDLNET_LIBRARY}" )
	set( libs "${libs};${SDLTTF_LIBRARY}" )
	set( libs "${libs};${LUA_LIBRARY}" )
	set( libs "${libs};${OPENGL_gl_LIBRARY}" )
	set( libs "${libs};${OPENGL_glu_LIBRARY}" )
	set( libs "${libs};${OPENAL_LIBRARY}" )

	set( libs "${libs};-lvorbisfile" )
	set( libs "${libs};-lalut" )

	if( OZ_XML_CONFIG )
		set( includes "${includes} -I${LIBXML2_INCLUDE_DIR}" )
		set( libs "${libs};${LIBXML2_LIBRARIES}" )
	endif( OZ_XML_CONFIG )

	set( CMAKE_CXX_FLAGS_DEBUG "-g3" CACHE STRING "" FORCE )
	set( CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG -O3 -fomit-frame-pointer -ffast-math -fstrict-enums" CACHE STRING "" FORCE )
	set( CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O3 -fomit-frame-pointer -ffast-math -fstrict-enums" CACHE STRING "" FORCE )
	set( CMAKE_BUILD_TYPE Debug CACHE STRING "Debug RelWithDebInfo Release" )

endif( MSVC )

set( CMAKE_CXX_FLAGS "${flags} ${arch_flags} ${warnings} ${includes}" )
message( "${libs}" )
link_libraries( ${libs} )

# sources
add_subdirectory( src )
add_subdirectory( data )

# installation
if( OZ_INSTALL_LIBOZ )

	install( FILES
		src/oz/oz.hpp
		${CMAKE_BINARY_DIR}/src/oz/ozconfig.hpp
		src/oz/common.hpp
		src/oz/Exception.hpp
		src/oz/Alloc.hpp
		src/oz/Pool.hpp
		src/oz/AutoPtr.hpp
		src/oz/iterables.hpp
		src/oz/arrays.hpp
		src/oz/Pair.hpp
		src/oz/List.hpp
		src/oz/DList.hpp
		src/oz/Array.hpp
		src/oz/DArray.hpp
		src/oz/String.hpp
		src/oz/Vector.hpp
		src/oz/SVector.hpp
		src/oz/Sparse.hpp
		src/oz/HashIndex.hpp
		src/oz/HashString.hpp
		src/oz/Bitset.hpp
		src/oz/SBitset.hpp
		src/oz/Math.hpp
		src/oz/Vec3.hpp
		src/oz/Quat.hpp
		src/oz/Mat33.hpp
		src/oz/Mat44.hpp
		src/oz/Log.hpp
		src/oz/Config.hpp
		DESTINATION include/oz )

	install( FILES ${CMAKE_BINARY_DIR}/src/oz/liboz.a
		DESTINATION lib )

endif( OZ_INSTALL_LIBOZ )

if( OZ_INSTALL_OPENZONE )

	install( FILES ${CMAKE_BINARY_DIR}/src/client/openzone
		DESTINATION bin
		PERMISSIONS
		OWNER_READ OWNER_WRITE OWNER_EXECUTE
		GROUP_READ GROUP_EXECUTE
		WORLD_READ WORLD_EXECUTE )

endif( OZ_INSTALL_OPENZONE )
