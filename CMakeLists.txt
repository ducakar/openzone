cmake_minimum_required( VERSION 2.8 )

project( openzone C CXX )

include( CheckLibraryExists )

option( OZ_ENABLE_ASSERT "Enable assert and soft_assert macros" ON )
option( OZ_ALLOC_STATISTICS "Use overloads for default new and delete that make heap usage statistics" ON )
option( OZ_POOL_ALLOC "Enable PoolAlloc and new/delete overloads that use it. Otherwise a dummy class that calls malloc/free is used and no overloads occur." ON )
option( OZ_GNU_MATH "Use GCC math built-ins (no need to include <cmath> and faster calls in debug mode)" ON )
option( OZ_VERBOSE_CONFIG "Print warnings about missing or unused variables in configuration files" ON )
option( OZ_XML_CONFIG "Support for XML configuration files (Linux only)" OFF )
option( OZ_CROSS_MINGW "If MinGW is not used natively on Windows, but on Unix for cross-compiling." OFF )
option( OZ_BUILD "Optimise and dump resources where possible. Those can be used by a OZ_PREBUILT enabled build." OFF )
option( OZ_PREBUILT "If OpenZone optimised resources should be loaded instead of generic ones" OFF )
option( OZ_ENABLE_CYLINDER "Enable cylinder collision shape and physics" OFF )

check_library_exists( m sincosf "" OZ_HAVE_SINCOSF )

Find_Package( LibXml2 REQUIRED )
Find_Package( SDL REQUIRED )
Find_Package( SDL_image REQUIRED )
Find_Package( SDL_net REQUIRED )
Find_Package( SDL_ttf REQUIRED )
Find_Package( Lua51 REQUIRED )
Find_Package( OpenGL REQUIRED )
Find_Package( OpenAL REQUIRED )

if( NOT ALUT_INCLUDE_DIR )
	set( ALUT_INCLUDE_DIR "NOT-SET" CACHE PATH "" FORCE )
endif( NOT ALUT_INCLUDE_DIR )

if( NOT ALUT_LIBRARY )
	set( ALUT_LIBRARY "NOT-SET" CACHE FILEPATH "" FORCE )
endif( NOT ALUT_LIBRARY )

if( NOT OGG_INCLUDE_DIR )
	set( OGG_INCLUDE_DIR "NOT-SET" CACHE PATH "" FORCE )
endif( NOT OGG_INCLUDE_DIR )

if( NOT OGG_LIBRARY )
	set( OGG_LIBRARY "NOT-SET" CACHE FILEPATH "" FORCE )
endif( NOT OGG_LIBRARY )

if( NOT VORBIS_INCLUDE_DIR )
	set( VORBIS_INCLUDE_DIR "NOT-SET" CACHE PATH "" FORCE )
endif( NOT VORBIS_INCLUDE_DIR )

if( NOT VORBIS_LIBRARY )
	set( VORBIS_LIBRARY "NOT-SET" CACHE FILEPATH "" FORCE )
endif( NOT VORBIS_LIBRARY )

if( NOT VORBISFILE_INCLUDE_DIR )
	set( VORBISFILE_INCLUDE_DIR "NOT-SET" CACHE PATH "" FORCE )
endif( NOT VORBISFILE_INCLUDE_DIR )

if( NOT VORBISFILE_LIBRARY )
	set( VORBISFILE_LIBRARY "NOT-SET" CACHE FILEPATH "" FORCE )
endif( NOT VORBISFILE_LIBRARY )

if( NOT DIRENT_INCLUDE_DIR )
	set( DIRENT_INCLUDE_DIR "NOT-SET" CACHE PATH "" FORCE )
endif( NOT DIRENT_INCLUDE_DIR )

if( MSVC )

	set( OZ_MSVC ON )

	# compiler and linker flags
	set( warnings "/W3 /D _CRT_SECURE_NO_WARNINGS" )
	set( flags "/EHsc" )
	set( libs "" )
	set( arch_flags "" )

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
	include_directories( ${VORBISFILE_INCLUDE_DIR} )
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
	
	set( includes "" )
	set( includes "${includes} -I${SDL_INCLUDE_DIR}" )
	set( includes "${includes} -I${LUA_INCLUDE_DIR}" )
	set( includes "${includes} -I${OPENGL_INCLUDE_DIR}" )
	set( includes "${includes} -I${OPENAL_INCLUDE_DIR}" )
	
	set( libs "${SDL_LIBRARY}" )
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
	set( CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG -O3 -fomit-frame-pointer -ffast-math -fstrict-enums ${arch_flags}" CACHE STRING "" FORCE )
	set( CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O3 -fomit-frame-pointer -ffast-math -fstrict-enums ${arch_flags}" CACHE STRING "" FORCE )
	set( CMAKE_BUILD_TYPE Debug CACHE STRING "Debug RelWithDebInfo Release" )

endif( MSVC )

set( CMAKE_CXX_FLAGS "${flags} ${warnings} ${includes}" )

if( NOT CMAKE_BUILD_TYPE )
	set( CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type" FORCE )
endif()

if( CMAKE_BUILD_TYPE STREQUAL Debug )
	set( flags "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG}" )
elseif( CMAKE_BUILD_TYPE STREQUAL Release )
	set( flags "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELEASE}" )
elseif( CMAKE_BUILD_TYPE STREQUAL RelWithDebInfo )
	set( flags "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELWITHDEBINFO}" )
else()
	set( flags "${CMAKE_CXX_FLAGS}" )
endif()

# sources
add_subdirectory( src )
add_subdirectory( data )

# installation
install( FILES
	src/oz/oz.hpp
	src/oz/ozconfig.hpp
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
	
install( FILES src/oz/liboz.a
	DESTINATION lib )
	
install( FILES src/client/openzone
	DESTINATION bin
	PERMISSIONS
	OWNER_READ OWNER_WRITE OWNER_EXECUTE
	GROUP_READ GROUP_EXECUTE
	WORLD_READ WORLD_EXECUTE )
