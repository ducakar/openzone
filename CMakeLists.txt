cmake_minimum_required( VERSION 2.8 )

include( CheckLibraryExists )

project( openzone C CXX )

option( OZ_ENABLE_ASSERT "Enable assertions (lots of sanity checks, for debug purpuses)" ON )
option( OZ_GNU_MATH "Use GCC math builtins (no need to include <cmath> and faster calls in debug mode)" ON )
option( OZ_ALLOC_STATISTICS "Use overloads for default new and delete that make heap usage statistics" ON )
option( OZ_POOL_ALLOC "Enable PoolAlloc and new/delete overloads that use it. Otherwise a dummy class that calls malloc/free is used and no overloads occur." OFF )
option( OZ_VERBOSE_CONFIG "Print warnings about missing or unused variables in config files" ON )
option( OZ_XML_CONFIG "Support for XML configuration files (Linux only)" OFF )
option( OZ_CROSS_MINGW "MinGW is not used natively from Windows, but from Unix for cross-compiling." OFF )

Find_Package( SDL REQUIRED )
Find_Package( SDL_image REQUIRED )
Find_Package( SDL_net REQUIRED )
Find_Package( SDL_ttf REQUIRED )
Find_Package( Lua51 REQUIRED )
Find_Package( OpenGL REQUIRED )
Find_Package( GLU REQUIRED )
Find_Package( OpenAL REQUIRED )

if( MSVC )

  	set( OZ_MSVC ON )

	# compiler and linker flags
	set( warnings "/W3 /D _CRT_SECURE_NO_WARNINGS" )
	set( flags "/EHsc" )
	set( libs "" )
	set( arch_flags "" )

	# set platform-specific includes and library names
	include_directories( deps/include )
	include_directories( deps/include/SDL )
	include_directories( deps/include/AL )
	set( libs "${libs} /LIBPATH:deps/lib" )
	
else( MSVC )
 
	# compiler and linker flags
	set( warnings "-Wall -Wextra -Winvalid-pch -Wold-style-cast -Wconversion -Wdisabled-optimization" )
	set( warnings "${warnings} -Wredundant-decls -Woverloaded-virtual -Wnon-virtual-dtor -Wsign-promo" )
	set( warnings "${warnings} -Wctor-dtor-privacy -Wstrict-overflow" )
	set( flags "-pedantic -std=c++0x -fmessage-length=0 -pipe" )
	set( libs "-lSDL -lSDL_image -lSDL_ttf -lSDL_net -lvorbisfile -llua" )
	set( arch_flags "-march=i686 -mtune=generic -mmmx -msse -mfpmath=both" )

	if( NOT OZ_CROSS_MINGW )
		set( CMAKE_FIND_ROOT_PATH "C:/MinGW" )
	endif( NOT OZ_CROSS_MINGW )

	# set platform-specific includes and library names
	if( WIN32 )
		set( libs "-lmingw32 -lSDLmain ${libs} -lopengl32 -lglu32 -lOpenAL32 -lalut" )
		set( OZ_MINGW ON )
		foreach( ROOT ${CMAKE_FIND_ROOT_PATH} )
			set( flags "${flags} -I${ROOT}/include" )
			set( flags "${flags} -I${ROOT}/include/SDL" )
			set( libs "${libs} -L${ROOT}/lib" )
		endforeach()
	else()
		set( flags "${flags} -I/usr/include/SDL" )
		set( libs "${libs} -lGL -lGLU -lopenal -lalut" )
		# libxml is only supported on linux
		if( OZ_XML_CONFIG )
			set( flags "${flags} -I/usr/include/libxml2" )
			set( libs "${libs} -lxml2" )
		endif()
	endif()

	set( CMAKE_CXX_FLAGS_DEBUG "-g3" CACHE STRING "" FORCE )
	set( CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG -O3 -fomit-frame-pointer -ffast-math ${arch_flags}" CACHE STRING "" FORCE )
	set( CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O3 -fomit-frame-pointer -ffast-math ${arch_flags}" CACHE STRING "" FORCE )
	set( CMAKE_BUILD_TYPE Debug CACHE STRING "Debug RelWithDebInfo Release" )

endif( MSVC )

set( CMAKE_CXX_FLAGS "${warnings} ${flags} ${arch_flags}" )

if( MINGW )
	set( CMAKE_CXX_STANDARD_LIBRARIES "${CMAKE_CXX_STANDARD_LIBRARIES} ${libs}" )
else()
	set( CMAKE_EXE_LINKER_FLAGS "${libs}" )
endif()

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

# installation
install( FILES src/client/openzone
	DESTINATION bin/openzone
	PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
				GROUP_READ GROUP_EXECUTE
				WORLD_READ WORLD_EXECUTE )

# sources
add_subdirectory( src )
add_subdirectory( data )
