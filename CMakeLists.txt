cmake_minimum_required( VERSION 2.8 )

if( "${CMAKE_CURRENT_SOURCE_DIR}" STREQUAL "${CMAKE_CURRENT_BINARY_DIR}" )
  message( FATAL_ERROR "You should not build in your source directory! (You may now safely delete 'CMakeFiles/' and 'CMakeCache.txt'.)" )
endif()

project( openzone C CXX )

set( OZ_APPLICATION_NAME "openzone" CACHE STRING "" FORCE )
set( OZ_APPLICATION_VERSION "0.2.80" CACHE STRING "" FORCE )
set( OZ_APPLICATION_TITLE "OpenZone" CACHE STRING "" FORCE )

option( OZ_TRACK_LEAKS "Save stack trace on operator new calls and track memory leaks." ON )

option( OZ_NETWORKING "Enable networking." OFF )
option( OZ_GL_COMPATIBLE "Use OpenGL 2.1 plus FBOs (roughly OpenGL ES 2.0) instead of OpenGL 3.0." ON )

option( OZ_INSTALL_LIBOZ "Install liboz as a standalone library" OFF )
option( OZ_INSTALL_CLIENT "Install openzone client" OFF )
option( OZ_INSTALL_TOOLS "Install ozBuild tool" OFF )
option( OZ_INSTALL_DATA "Install prebuilt game contents" OFF )
option( OZ_INSTALL_DATA_SRC "Install sources of prebuild game data" OFF )
option( OZ_INSTALL_ICONS "Install icon in applications menu (Linux only)" OFF )
option( OZ_INSTALL_INFO "Install README, AUTHORS, COPYING ..." OFF )
option( OZ_INSTALL_STANDALONE "Install layout appropriate for launching from its own folder" OFF )

if( NOT CMAKE_BUILD_TYPE )
  set( CMAKE_BUILD_TYPE "Debug" CACHE STRING "Debug RelWithDebInfo Release ReleaseGen ReleaseUse" FORCE )
endif()

if( WIN32 )
  set( OZ_SYSTEM_NAME "${CMAKE_SYSTEM_NAME}-i686" CACHE STRING "" FORCE )
else()
  set( OZ_SYSTEM_NAME "${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}" CACHE STRING "" FORCE )
endif()

mark_as_advanced( OZ_APPLICATION_NAME )
mark_as_advanced( OZ_APPLICATION_VERSION )
mark_as_advanced( OZ_APPLICATION_TITLE )
mark_as_advanced( OZ_SYSTEM_NAME )

include( cmake/pch.cmake )
include( CheckLibraryExists )

Find_Package( SDL REQUIRED )
Find_Package( SDL_ttf REQUIRED )
Find_Package( PhysFS REQUIRED )
Find_Package( Lua51 REQUIRED )
Find_Package( OpenGL REQUIRED )
Find_Package( OpenAL REQUIRED )

if( NOT DEFINED FREEIMAGE_INCLUDE_DIR )
  set( FREEIMAGE_INCLUDE_DIR "" CACHE PATH "" FORCE )
endif()
if( NOT DEFINED OGG_INCLUDE_DIR )
  set( OGG_INCLUDE_DIR "" CACHE PATH "" FORCE )
endif()
if( NOT DEFINED VORBIS_INCLUDE_DIR )
  set( VORBIS_INCLUDE_DIR "" CACHE PATH "" FORCE )
endif()
if( NOT DEFINED VORBISFILE_INCLUDE_DIR )
  set( VORBISFILE_INCLUDE_DIR "" CACHE PATH "" FORCE )
endif()
if( NOT DEFINED FAAD_INCLUDE_DIR )
  set( FAAD_INCLUDE_DIR "" CACHE PATH "" FORCE )
endif()
if( NOT DEFINED MAD_INCLUDE_DIR )
  set( MAD_INCLUDE_DIR "" CACHE PATH "" FORCE )
endif()

find_library( FREEIMAGE_LIBRARY freeimage NAMES FreeImage )
find_library( VORBISFILE_LIBRARY vorbisfile )

mark_as_advanced( SDL_INCLUDE_DIR )
mark_as_advanced( SDL_LIBRARY )
mark_as_advanced( SDLMAIN_LIBRARY )
mark_as_advanced( SDLTTF_INCLUDE_DIR )
mark_as_advanced( SDLTTF_LIBRARY )
mark_as_advanced( PHYSFS_INCLUDE_DIR )
mark_as_advanced( PHYSFS_LIBRARY )
mark_as_advanced( FREEIMAGE_INCLUDE_DIR )
mark_as_advanced( FREEIMAGE_LIBRARY )
mark_as_advanced( OGG_INCLUDE_DIR )
mark_as_advanced( VORBIS_INCLUDE_DIR )
mark_as_advanced( VORBISFILE_INCLUDE_DIR )
mark_as_advanced( VORBISFILE_LIBRARY )
mark_as_advanced( FAAD_INCLUDE_DIR )
mark_as_advanced( MAD_INCLUDE_DIR )

if( WIN32 )
  find_library( INTL_LIBRARY intl )
  find_library( WINMM_LIBRARY winmm )

  mark_as_advanced( MINGW32_LIBRARY )
  mark_as_advanced( INTL_LIBRARY )
  mark_as_advanced( WINMM_LIBRARY )
endif()

# detect clang
string( REGEX MATCH "clang\\+\\+$" compiler_base "${CMAKE_CXX_COMPILER}" )
if( "${compiler_base}" STREQUAL "clang++" )
  set( OZ_CLANG ON )
endif()

include_directories( ${SDL_INCLUDE_DIR} )
include_directories( ${SDLTTF_INCLUDE_DIR} )
include_directories( ${PHYSFS_INCLUDE_DIR} )
include_directories( ${LUA_INCLUDE_DIR} )
include_directories( ${OPENGL_INCLUDE_DIR} )
include_directories( ${OPENAL_INCLUDE_DIR} )
include_directories( ${FREEIMAGE_INCLUDE_DIR} )
include_directories( ${OGG_INCLUDE_DIR} )
include_directories( ${VORBIS_INCLUDE_DIR} )
include_directories( ${VORBISFILE_INCLUDE_DIR} )
include_directories( ${FAAD_INCLUDE_DIR} )
include_directories( ${MAD_INCLUDE_DIR} )

if( WIN32 )
  include_directories( ./include )
endif()

if( WIN32 )
  set( libs "${WINMM_LIBRARY};${INTL_LIBRARY};${PHYSFS_LIBRARY}" )
else()
  set( libs "-lrt;-lpthread;-lpulse-simple;${PHYSFS_LIBRARY}" )
endif()

set( libs_client "${libs};${SDL_LIBRARY}" )
set( libs_client "${libs_client};${SDLTTF_LIBRARY}" )
set( libs_client "${libs_client};${LUA_LIBRARY}" )
set( libs_client "${libs_client};${OPENGL_gl_LIBRARY}" )
set( libs_client "${libs_client};${OPENAL_LIBRARY}" )
set( libs_client "${libs_client};${VORBISFILE_LIBRARY}" )

if( WIN32 )
  set( libs_client "${libs_client};${FAAD_LIBRARY}" )
  set( libs_client "${libs_client};${MAD_LIBRARY}" )
endif()

set( libs_tools "${libs_client};${FREEIMAGE_LIBRARY}" )

# compiler and linker flags
set( flags "-pipe -std=c++0x -fstrict-enums -ffast-math -freciprocal-math" )
set( warnings "-Wall -Wextra -Wconversion -Woverloaded-virtual -Wnon-virtual-dtor -Winvalid-pch" )
set( arch_flags "-msse3 -mfpmath=sse" )

if( OZ_CLANG )
  set( flags "-Qunused-arguments ${flags}" )
endif()

set( CMAKE_BUILD_TYPE Debug CACHE STRING "Debug RelWithDebInfo Release ReleaseGen ReleaseUse" )

if( WIN32 )
  set( CMAKE_CXX_FLAGS_DEBUG "-g3 -O0" CACHE STRING "" FORCE )
  set( CMAKE_CXX_FLAGS_RELWITHDEBINFO "-g3 -O3" CACHE STRING "" FORCE )
  set( CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG -O3" CACHE STRING "" FORCE )
  set( CMAKE_CXX_FLAGS_RELEASEGEN "-DNDEBUG -O3 -fprofile-generate" CACHE STRING "" FORCE )
  set( CMAKE_CXX_FLAGS_RELEASEUSE "-DNDEBUG -O3 -fprofile-use" CACHE STRING "" FORCE )

  set( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--enable-auto-import" )
elseif( OZ_CLANG )
  set( CMAKE_CXX_FLAGS_DEBUG "-g3 -O0 -fstack-protector" CACHE STRING "" FORCE )
  set( CMAKE_CXX_FLAGS_RELWITHDEBINFO "-g3 -O3 -fstack-protector" CACHE STRING "" FORCE )
  set( CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG -O3" CACHE STRING "" FORCE )
  set( CMAKE_CXX_FLAGS_RELEASEGEN "-DNDEBUG -O3 -fprofile-generate" CACHE STRING "" FORCE )
  set( CMAKE_CXX_FLAGS_RELEASEUSE "-DNDEBUG -O3 -fprofile-use" CACHE STRING "" FORCE )
else()
  set( CMAKE_CXX_FLAGS_DEBUG "-g3 -O0 -fstack-protector" CACHE STRING "" FORCE )
  set( CMAKE_CXX_FLAGS_RELWITHDEBINFO "-g3 -O3 -fstack-protector" CACHE STRING "" FORCE )
  set( CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG -O3 -flto" CACHE STRING "" FORCE )
  set( CMAKE_CXX_FLAGS_RELEASEGEN "-DNDEBUG -O3 -flto -fprofile-generate" CACHE STRING "" FORCE )
  set( CMAKE_CXX_FLAGS_RELEASEUSE "-DNDEBUG -O3 -flto -fprofile-use" CACHE STRING "" FORCE )
endif()

mark_as_advanced( CMAKE_CXX_FLAGS_RELEASEGEN )
mark_as_advanced( CMAKE_CXX_FLAGS_RELEASEUSE )
mark_as_advanced( CMAKE_EXE_LINKER_FLAGS_RELEASEGEN )
mark_as_advanced( CMAKE_EXE_LINKER_FLAGS_RELEASEUSE )

set( CMAKE_CXX_FLAGS "${flags} ${arch_flags} ${warnings} ${CMAKE_CXX_FLAGS}" )

# construct flags strings for BuildInfo
string( TOUPPER "CMAKE_CXX_FLAGS_${CMAKE_BUILD_TYPE}" OZ_CXX_FLAGS )
string( TOUPPER "CMAKE_EXE_LINKER_FLAGS_${CMAKE_BUILD_TYPE}" OZ_EXE_LINKER_FLAGS )

set( libsString "" )
foreach( lib ${libs} )
  set( libsString "${libsString} ${lib}" )
endforeach()

set( OZ_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${${OZ_CXX_FLAGS}}" )
set( OZ_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${${OZ_EXE_LINKER_FLAGS}} ${libsString}" )

string( STRIP "${OZ_CXX_FLAGS}" OZ_CXX_FLAGS )
string( STRIP "${OZ_EXE_LINKER_FLAGS}" OZ_EXE_LINKER_FLAGS )

# sources
add_subdirectory( src )

# installation
include( cmake/CPackConfig.cmake )
include( cmake/data_install.cmake )
