cmake_minimum_required( VERSION 2.6 )

include( CheckLibraryExists )

project( openzone C CXX )

check_library_exists( m sincosf "" OZ_HAVE_SINCOSF )
option( OZ_ALLOC_STATISTICS 
    "Use overloads for default new and delete that make heap usage statistics" OFF )
option( OZ_POOL_ALLOC "Enable PoolAlloc and new/delete overloads that use it. Otherwise a dummy class that calls malloc/free is used and no overloads occur." ON )
option( OZ_XML_CONFIG "Support for XML configuration files (Linux only)" OFF )

# compiler and linker flags
set( warnings "-Wall -Wextra -Winvalid-pch -Wold-style-cast -Wconversion -Wdisabled-optimization" )
set( warnings "${warnings} -Wredundant-decls -Woverloaded-virtual -Wnon-virtual-dtor -Wsign-promo" )
set( warnings "${warnings} -Wctor-dtor-privacy -Wstrict-overflow" )
set( flags "-fmessage-length=0 -pipe" )
set( libs "-lSDL -lSDL_image -lSDL_ttf -lSDL_net -lvorbisfile -llua" )
set( arch_flags "-march=i686 -mtune=generic -mmmx -msse -mfpmath=sse" )

# set platform-specific includes and library names
if( WIN32 )
    set( libs "-lmingw32 -lSDLmain ${libs} -lopengl32 -lglu32 -lOpenAL32 -lalut" )
    set( OZ_MINGW32 ON )
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

set( CMAKE_CXX_FLAGS "${warnings} ${flags}" )

if( WIN32 )
    set( CMAKE_CXX_STANDARD_LIBRARIES "${CMAKE_CXX_STANDARD_LIBRARIES} ${libs}" )
else()
    set( CMAKE_EXE_LINKER_FLAGS "${libs}" )
endif()

set( CMAKE_CXX_FLAGS_DEBUG "-g3" CACHE STRING "" FORCE )
set( CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG -O3 -fomit-frame-pointer -ffast-math ${arch_flags}" CACHE STRING "" FORCE )
set( CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O3 -fomit-frame-pointer -ffast-math ${arch_flags}" CACHE STRING "" FORCE )
set( CMAKE_BUILD_TYPE Debug CACHE STRING "Debug RelWithDebInfo Release" )

# write configuration
configure_file( configuration.h.in configuration.h )

# installation
install( FILES src/client/openzone
         DESTINATION bin/openzone
         PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
                     GROUP_READ GROUP_EXECUTE
                     WORLD_READ WORLD_EXECUTE )

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

add_custom_target( precompiled DEPENDS precompiled.h.gch )
add_custom_command( OUTPUT precompiled.h.gch
                    DEPENDS base precompiled.h
                    COMMAND rm -f precompiled.h.gch &&
                            ${CMAKE_CXX_COMPILER} `echo ${flags}` -I. precompiled.h )

# sources
include_directories( . )
include_directories( src )
add_subdirectory( src )
add_subdirectory( data )
