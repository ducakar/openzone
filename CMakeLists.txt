cmake_minimum_required( VERSION 2.8 )

if( "${CMAKE_CURRENT_SOURCE_DIR}" STREQUAL "${CMAKE_CURRENT_BINARY_DIR}" )
  message( FATAL_ERROR "You should not build in your source directory! (You may now safely delete 'CMakeFiles/' and 'CMakeCache.txt'.)" )
endif()

project( openzone C CXX )

set( OZ_APPLICATION_NAME "openzone" CACHE STRING "" FORCE )
set( OZ_APPLICATION_VERSION "0.2.80" CACHE STRING "" FORCE )
set( OZ_APPLICATION_TITLE "OpenZone" CACHE STRING "" FORCE )

mark_as_advanced( OZ_APPLICATION_NAME )
mark_as_advanced( OZ_APPLICATION_VERSION )
mark_as_advanced( OZ_APPLICATION_TITLE )

option( OZ_TRACE_LEAKS "Use overloads for default new and delete that saves stacktrace for each new call and trace memory leaks." OFF )
option( OZ_BIG_ENDIAN_STREAM "Use big-endian byte order for binary files and networking" OFF )

option( OZ_NETWORKING "Enable networking." OFF )
option( OZ_GL_COMPATIBLE "Use common subset of OpenGL 2.1, OpenGL 3.0, OpenGL ES 2.0, no texture compression, GLSL 1.20." ON )
option( OZ_BUMPMAP "Enable bumpmap support (not implemented yet, only vertex format for now)." OFF )

option( OZ_BUILD_TESTS "Build tests." OFF )
option( OZ_INSTALL_LIBOZ "Install liboz." OFF )
option( OZ_INSTALL_OPENZONE "Install OpenZone engine." OFF )
option( OZ_INSTALL_TOOLS "Install ozPrebuild tool for prebuilding data." OFF )
option( OZ_INSTALL_INFO "Install info files (README, COPYING, ...)." OFF )
option( OZ_INSTALL_ICONS "Install icons in Linux-like systems." OFF )
option( OZ_INSTALL_DATA "Install prebuild data needed to run game." OFF )
option( OZ_INSTALL_DATA_SRC "Install sources of prebuilt data (for content development)." OFF )
option( OZ_INSTALL_STANDALONE "Install some support files if a standalone archive." OFF )

if( NOT DEFINED OZ_RC_DIR )
  if( WIN32 )
    set( OZ_RC_DIR "OpenZone" CACHE STRING "" FORCE )
  else()
    set( OZ_RC_DIR ".config/openzone" CACHE STRING "" FORCE )
  endif()
endif()

if( NOT DEFINED OZ_CLIENT_LOG_FILE )
  set( OZ_CLIENT_LOG_FILE "STDOUT" CACHE STRING "" FORCE )
endif()

if( NOT DEFINED OZ_CLIENT_CONFIG_FILE )
  set( OZ_CLIENT_CONFIG_FILE "client.rc" CACHE STRING "" FORCE )
endif()

if( NOT DEFINED OZ_SYSTEM_NAME )
  if( WIN32 )
    set( OZ_SYSTEM_NAME "${CMAKE_SYSTEM_NAME}-i686" CACHE STRING "" FORCE )
  else()
    set( OZ_SYSTEM_NAME "${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}" CACHE STRING "" FORCE )
  endif()
endif()

if( NOT CMAKE_BUILD_TYPE )
  set( CMAKE_BUILD_TYPE "Debug" CACHE STRING "Debug RelWithDebInfo Release ReleaseGen ReleaseUse" FORCE )
endif()

include( cmake/pch.cmake )
include( cmake/pack_config.cmake )
include( CheckLibraryExists )

Find_Package( SDL REQUIRED )

mark_as_advanced( SDL_INCLUDE_DIR )
mark_as_advanced( SDL_LIBRARY )
mark_as_advanced( SDLMAIN_LIBRARY )

Find_Package( SDL_ttf REQUIRED )

mark_as_advanced( SDLTTF_INCLUDE_DIR )
mark_as_advanced( SDLTTF_LIBRARY )

Find_Package( Lua51 REQUIRED )
Find_Package( OpenGL REQUIRED )
Find_Package( OpenAL REQUIRED )

if( NOT DEFINED OGG_INCLUDE_DIR )
  set( OGG_INCLUDE_DIR "" CACHE PATH "" FORCE )
endif()
if( NOT DEFINED VORBIS_INCLUDE_DIR )
  set( VORBIS_INCLUDE_DIR "" CACHE PATH "" FORCE )
endif()
if( NOT DEFINED VORBISFILE_INCLUDE_DIR )
  set( VORBISFILE_INCLUDE_DIR "" CACHE PATH "" FORCE )
endif()
if( NOT DEFINED MAD_INCLUDE_DIR )
  set( MAD_INCLUDE_DIR "" CACHE PATH "" FORCE )
endif()

find_library( VORBISFILE_LIBRARY vorbisfile )

mark_as_advanced( OGG_INCLUDE_DIR )
mark_as_advanced( VORBIS_INCLUDE_DIR )
mark_as_advanced( MAD_INCLUDE_DIR )
mark_as_advanced( VORBISFILE_INCLUDE_DIR )
mark_as_advanced( VORBISFILE_LIBRARY )

if( OZ_NETWORKING )
  Find_Package( SDL_net REQUIRED )

  mark_as_advanced( SDLNET_INCLUDE_DIR )
  mark_as_advanced( SDLNET_LIBRARY )
endif()

mark_as_advanced( MAD_INCLUDE_DIR )

if( WIN32 )
  find_library( INTL_LIBRARY intl )
  find_library( WINMM_LIBRARY winmm )

  mark_as_advanced( MINGW32_LIBRARY )
  mark_as_advanced( INTL_LIBRARY )
  mark_as_advanced( WINMM_LIBRARY )
else()
  find_library( PULSE_SIMPLE_LIBRARY pulse-simple )

  mark_as_advanced( PULSE_SIMPLE_LIBRARY )
endif()

Find_Package( SDL_image REQUIRED )
Find_Package( PNG REQUIRED )
Find_Package( JPEG REQUIRED )

mark_as_advanced( SDLIMAGE_INCLUDE_DIR )
mark_as_advanced( SDLIMAGE_LIBRARY )

# detect clang
string( REGEX MATCH "clang\\+\\+$" compiler_base "${CMAKE_CXX_COMPILER}" )
if( "${compiler_base}" STREQUAL "clang++" )
  set( OZ_CLANG ON )
endif()

# detect platform
if( WIN32 )
  set( OZ_MINGW ON )
endif()

include_directories( ${SDL_INCLUDE_DIR} )
include_directories( ${SDLIMAGE_INCLUDE_DIR} )
include_directories( ${SDLTTF_INCLUDE_DIR} )
include_directories( ${LUA_INCLUDE_DIR} )
include_directories( ${OPENGL_INCLUDE_DIR} )
include_directories( ${OPENAL_INCLUDE_DIR} )
include_directories( ${OGG_INCLUDE_DIR} )
include_directories( ${VORBIS_INCLUDE_DIR} )
include_directories( ${VORBISFILE_INCLUDE_DIR} )

if( OZ_MINGW )
  include_directories( ./include )
endif()

if( OZ_MINGW )
  set( libs "${WINMM_LIBRARY};${INTL_LIBRARY}" )
else()
  set( libs "-ldl" )
endif()

set( libs "${libs};${SDL_LIBRARY}" )
set( libs "${libs};${SDLTTF_LIBRARY}" )
set( libs "${libs};${LUA_LIBRARY}" )
set( libs "${libs};${OPENGL_gl_LIBRARY}" )
set( libs "${libs};${OPENAL_LIBRARY}" )
set( libs "${libs};${VORBISFILE_LIBRARY}" )

if( OZ_NETWORK )
  include_directories( ${SDLNET_INCLUDE_DIR} )
  set( libs "${libs};${SDLNET_LIBRARY}" )
endif()

set( libs_tools "${libs};${SDLIMAGE_LIBRARY}" )

# compiler and linker flags
set( flags "-pipe -std=c++0x -pedantic-errors -ffast-math" )
set( warnings "-Wall -Wextra -Winvalid-pch -Wdisabled-optimization -Wattributes" )
set( warnings "${warnings} -Wconversion -Wsign-conversion -Wsign-promo -Wcast-align" )
set( warnings "${warnings} -Woverloaded-virtual -Wnon-virtual-dtor" )
set( arch_flags "-msse3" )

if( OZ_CLANG )
  set( warnings "${warnings} -Wno-invalid-offsetof" )
else()
  set( flags "${flags} -fstrict-enums -freciprocal-math" )
  set( warnings "${warnings} -Wlogical-op" )
  set( arch_flags "${arch_flags} -mfpmath=sse" )
endif()

set( CMAKE_BUILD_TYPE Debug CACHE STRING "Debug RelWithDebInfo ReleaseGen Release" )

if( OZ_MINGW )
  set( CMAKE_CXX_FLAGS_DEBUG "-g3" CACHE STRING "" FORCE )
  set( CMAKE_CXX_FLAGS_RELWITHDEBINFO "-g3 -Ofast -fdata-sections -ffunction-sections" CACHE STRING "" FORCE )
  set( CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG -Ofast -fdata-sections -ffunction-sections" CACHE STRING "" FORCE )
  set( CMAKE_CXX_FLAGS_RELEASEGEN "-DNDEBUG -Ofast -fdata-sections -ffunction-sections -fprofile-generate" CACHE STRING "" FORCE )
  set( CMAKE_CXX_FLAGS_RELEASEUSE "-DNDEBUG -Ofast -fdata-sections -ffunction-sections -fprofile-use" CACHE STRING "" FORCE )

  set( CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO "-Wl,--gc-sections -Wl,--enable-auto-import" CACHE STRING "" FORCE )
  set( CMAKE_EXE_LINKER_FLAGS_RELEASE "-Wl,--gc-sections -Wl,--enable-auto-import" CACHE STRING "" FORCE )
  set( CMAKE_EXE_LINKER_FLAGS_RELEASEGEN "-fprofile-generate -Wl,--gc-sections -Wl,--enable-auto-import" CACHE STRING "" FORCE )
  set( CMAKE_EXE_LINKER_FLAGS_RELEASEUSE "-fprofile-use -Wl,--gc-sections -Wl,--enable-auto-import" CACHE STRING "" FORCE )

  set( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--enable-auto-import" )
elseif( OZ_CLANG )
  set( CMAKE_CXX_FLAGS_DEBUG "-g3" CACHE STRING "" FORCE )
  set( CMAKE_CXX_FLAGS_RELWITHDEBINFO "-g3 -O3 -fdata-sections -ffunction-sections" CACHE STRING "" FORCE )
  set( CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG -O3 -fdata-sections -ffunction-sections" CACHE STRING "" FORCE )
  set( CMAKE_CXX_FLAGS_RELEASEGEN "-DNDEBUG -O3 -fdata-sections -ffunction-sections -fprofile-generate" CACHE STRING "" FORCE )
  set( CMAKE_CXX_FLAGS_RELEASEUSE "-DNDEBUG -O3 -fdata-sections -ffunction-sections -fprofile-use" CACHE STRING "" FORCE )

  set( CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO "-Wl,--gc-sections" CACHE STRING "" FORCE )
  set( CMAKE_EXE_LINKER_FLAGS_RELEASE "-Wl,--gc-sections" CACHE STRING "" FORCE )
  set( CMAKE_EXE_LINKER_FLAGS_RELEASEGEN "-fprofile-generate -Wl,--gc-sections" CACHE STRING "" FORCE )
  set( CMAKE_EXE_LINKER_FLAGS_RELEASEUSE "-fprofile-use -Wl,--gc-sections" CACHE STRING "" FORCE )
else()
  set( CMAKE_CXX_FLAGS_DEBUG "-g3 -fstack-protector-all" CACHE STRING "" FORCE )
  set( CMAKE_CXX_FLAGS_RELWITHDEBINFO "-g3 -Ofast -fstack-protector-all -fdata-sections -ffunction-sections" CACHE STRING "" FORCE )
  set( CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG -Ofast -flto -fdata-sections -ffunction-sections" CACHE STRING "" FORCE )
  set( CMAKE_CXX_FLAGS_RELEASEGEN "-DNDEBUG -Ofast -flto -fdata-sections -ffunction-sections -fprofile-generate" CACHE STRING "" FORCE )
  set( CMAKE_CXX_FLAGS_RELEASEUSE "-DNDEBUG -Ofast -flto -fdata-sections -ffunction-sections -fprofile-use" CACHE STRING "" FORCE )

  set( CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO "-Wl,--gc-sections" CACHE STRING "" FORCE )
  set( CMAKE_EXE_LINKER_FLAGS_RELEASE "-flto -Wl,--gc-sections" CACHE STRING "" FORCE )
  set( CMAKE_EXE_LINKER_FLAGS_RELEASEGEN "-flto -fprofile-generate -Wl,--gc-sections" CACHE STRING "" FORCE )
  set( CMAKE_EXE_LINKER_FLAGS_RELEASEUSE "-flto -fprofile-use -Wl,--gc-sections" CACHE STRING "" FORCE )
endif()

mark_as_advanced( CMAKE_CXX_FLAGS_RELEASEGEN )
mark_as_advanced( CMAKE_CXX_FLAGS_RELEASEUSE )
mark_as_advanced( CMAKE_EXE_LINKER_FLAGS_RELEASEGEN )
mark_as_advanced( CMAKE_EXE_LINKER_FLAGS_RELEASEUSE )

set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${flags} ${arch_flags} ${warnings}" )

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

# data installation
include( cmake/data_install.cmake )
