cmake_minimum_required( VERSION 2.6 )

include( CheckLibraryExists )

project( openzone C CXX )

option( OZ_XML_CONFIG "Support for XML configuration files" ON )

if( NOT CMAKE_BUILD_TYPE )
    set( CMAKE_BUILD_TYPE Debug FORCE )
endif()

check_library_exists( m sincosf "" OZ_HAVE_SINCOSF )

# compiler and linker flags
set( WARNINGS "-Wall -Wextra -Winvalid-pch -Wold-style-cast -Wdisabled-optimization" )
set( WARNINGS "${WARNINGS} -Woverloaded-virtual -Wnon-virtual-dtor" ) 
set( FLAGS "-fno-rtti -pipe" )
set( LIBS "-lSDL -lSDL_image -lSDL_ttf -lSDL_net -lvorbisfile -llua" )
set( ARCH_FLAGS "-march=i686 -mtune=generic -mmmx -msse -mfpmath=sse" )

# set platform-specific includes and library names
if( WIN32 )
    set( OZ_MINGW32 ON )
    foreach( ROOT ${CMAKE_FIND_ROOT_PATH} )
        set( FLAGS "${FLAGS} -I${ROOT}/include" )
        set( FLAGS "${FLAGS} -I${ROOT}/include/SDL" )
        set( LIBS "${LIBS} -L${ROOT}/lib" )
    endforeach()
    set( LIBS "-lmingw32 -lSDLmain ${LIBS} -lopengl32 -lglu32 -lOpenAL32 -lalut" )
else()
    set( FLAGS "${FLAGS} -I/usr/include/SDL" )
    set( LIBS "${LIBS} -lGL -lGLU -lopenal -lalut" )
    # libxml is only supported on linux 
    if( OZ_XML_CONFIG )
        set( FLAGS "${FLAGS} -I/usr/include/libxml2" )
        set( LIBS "${LIBS} -lxml2" )
    endif()
endif()

set( CMAKE_CXX_FLAGS "${WARNINGS} ${FLAGS}" )
set( CMAKE_EXE_LINKER_FLAGS "${LIBS}" )

set( CMAKE_CXX_FLAGS_DEBUG "-g3" CACHE STRING "" FORCE )
set( CMAKE_CXX_FLAGS_RELEASE "-O3 -fomit-frame-pointer -ffast-math ${ARCH_FLAGS}" CACHE STRING "" FORCE ) 

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
    set( FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG}" )
elseif( CMAKE_BUILD_TYPE STREQUAL Release )
    set( FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELEASE}" )
else()
    set( FLAGS "${CMAKE_CXX_FLAGS}" )
endif()

add_custom_target( precompiled ALL DEPENDS precompiled.h.gch )
add_custom_command( OUTPUT precompiled.h.gch
                    DEPENDS base precompiled.h
                    COMMAND rm -f precompiled.h.gch &&
                            ${CMAKE_CXX_COMPILER} `echo ${FLAGS}` -I. precompiled.h )

# sources
include_directories( . )
include_directories( src )
add_subdirectory( src )
