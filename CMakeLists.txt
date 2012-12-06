cmake_minimum_required( VERSION 2.8 )

project( openzone C CXX )

#
# Configuration
#

set( OZ_VERSION 0.3.81 )

option( OZ_TRACK_ALLOCS "Track memory allocations to detect new/delete mismatches and memory leaks." OFF )
option( OZ_SIMD_MATH "Use SIMD implementation of linear algebra classes." OFF )
option( OZ_ODE "Build complete ozDynamics library (requires Open Dynamics Engine)." OFF )

option( OZ_LUAJIT "Use use LuaJIT 2.0 instead of official Lua." OFF )
option( OZ_SDL2 "Use SDL 2.0 instead of SDL 1.2." OFF )
option( OZ_GL_ES "Use OpenGL ES 2.0 instead of OpenGL 2.1+." OFF )
option( OZ_NONFREE "Enable support for building S3TC textures." OFF )

option( OZ_STANDALONE "Install layout appropriate for launching from its own folder." OFF )

#
# Internal configuration
#

include( cmake/PCH.cmake )
include( CheckCXXSourceCompiles )
include( CheckCXXCompilerFlag )
include( FindPkgConfig )

if( PLATFORM_EMBEDDED )
  set( BUILD_SHARED_LIBS OFF )
  set( OZ_GL_ES ON )
  set( OZ_STANDALONE OFF )
else()
  set( BUILD_SHARED_LIBS ON )
endif()

if( ANDROID )
  set( CMAKE_SYSTEM_NAME Android )
  set( OZ_SDL2 ON )
elseif( NACL )
  set( CMAKE_SYSTEM_NAME NaCl )
  set( OZ_SDL2 OFF )
elseif( WIN32 )
  set( OZ_STANDALONE ON )
endif()

set( OZ_PLATFORM_NAME "${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}" )

if( OZ_STANDALONE )
  set( CMAKE_INSTALL_PREFIX "/OpenZone-${OZ_VERSION}" )
  set( OZ_BINARY_SUBDIR "/${OZ_PLATFORM_NAME}" )
else()
  set( OZ_BINARY_SUBDIR "" )
endif()

if( ${CMAKE_CXX_COMPILER_ID} STREQUAL GNU )
  if( ${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 4.4 )
    message( FATAL_ERROR "GCC >= 4.4 and LLVM/Clang >= 3.0 are the only supported compilers." )
  endif()
elseif( ${CMAKE_CXX_COMPILER_ID} STREQUAL Clang )
  if( ${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 3.0 )
    message( FATAL_ERROR "GCC >= 4.4 and LLVM/Clang >= 3.0 are the only supported compilers." )
  endif()
else()
  message( FATAL_ERROR "GCC >= 4.4 and LLVM/Clang >= 3.0 are the only supported compilers." )
endif()

#
# Libraries
#

# Set-up include and library directories for different toolchains.
if( ANDROID )
  include_directories( SYSTEM ${PLATFORM_PREFIX}/usr/include ) # For KDevelop.
  include_directories( SYSTEM ${PLATFORM_PORTS_PREFIX}/usr/include )

  link_directories( ${PLATFORM_PORTS_PREFIX}/usr/lib )
elseif( NACL )
  include_directories( SYSTEM ${PLATFORM_PREFIX}/include ) # For KDevelop.
  include_directories( SYSTEM ${PLATFORM_PORTS_PREFIX}/usr/include )

  link_directories( ${PLATFORM_PORTS_PREFIX}/usr/lib )
elseif( WIN32 )
else()
  if( CMAKE_SYSTEM_NAME STREQUAL Linux )
    find_package( ALSA REQUIRED )
  endif()
  pkg_check_modules( PULSE_SIMPLE libpulse-simple REQUIRED )
endif()

find_package( PhysFS REQUIRED )
mark_as_advanced( PHYSFS_INCLUDE_DIR )
mark_as_advanced( PHYSFS_LIBRARY )

if( OZ_ODE )
  pkg_check_modules( ODE REQUIRED ode )
endif()

if( OZ_LUAJIT )
  pkg_check_modules( LUAJIT REQUIRED luajit )

  include_directories( SYSTEM ${LUAJIT_INCLUDE_DIRS} )
else()
  find_package( Lua51 REQUIRED )
endif()

if( OZ_SDL2 )
  pkg_check_modules( SDL2 REQUIRED sdl2 )
  pkg_check_modules( SDL2_TTF REQUIRED SDL2_ttf )

  include_directories( SYSTEM ${SDL2_INCLUDE_DIRS} )
else()
  find_package( SDL REQUIRED )
  find_package( SDL_ttf REQUIRED )

  mark_as_advanced( SDLMAIN_LIBRARY )
  mark_as_advanced( SDL_INCLUDE_DIR )
  mark_as_advanced( SDL_LIBRARY )

  include_directories( SYSTEM ${SDL_INCLUDE_DIR} )
endif()

if( OZ_GL_ES )
  # On embedded platform GLES is part of system libraries and is guaranteed to exist in toolchain or
  # platform directory, no need to explicitly check existence or to detect library names.
  if( NOT PLATFORM_EMBEDDED )
    pkg_check_modules( GLES2 REQUIRED glesv2 )
  endif()
else()
  find_package( OpenGL REQUIRED )
endif()

find_package( OpenAL REQUIRED )
pkg_check_modules( VORBISFILE REQUIRED vorbisfile )

# On embedded platforms linking is mostly static, so dependencies for all libraries must be linked
# too.
if( PLATFORM_EMBEDDED )
  pkg_check_modules( VORBIS REQUIRED vorbis ) # required by vorbisfile
  pkg_check_modules( OGG REQUIRED ogg )       # required by vorbisfile
  find_package( Freetype REQUIRED )           # required by SDL_ttf
  find_package( ZLIB REQUIRED )               # required by PhysicsFS & FreeType
endif()

if( NOT PLATFORM_EMBEDDED )
  find_library( FREEIMAGE_LIBRARY freeimage )
  mark_as_advanced( FREEIMAGE_LIBRARY )

  if( NOT FREEIMAGE_LIBRARY )
    message( FATAL_ERROR "FreeImage library missing" )
  endif()

  if( OZ_NONFREE )
    find_library( SQUISH_LIBRARY squish )
    mark_as_advanced( SQUISH_LIBRARY )

    if( NOT SQUISH_LIBRARY )
      message( FATAL_ERROR "libsquish missing (required by OZ_NONFREE)" )
    endif()
  endif()
endif()

mark_as_advanced( MINGW32_LIBRARY )

#
# Compiler flags
#

# Local libmad, faad and eSpeak headers.
include_directories( SYSTEM ./include )

if( ${CMAKE_CXX_COMPILER_ID} STREQUAL Clang )
  set( flags "-std=c++11 -pedantic" )

  # Use libc++ instead of libstdc++.
  # set( flags "${flags} -stdlib=libc++" )
  # AddressSanitizer memory checker
  # set( flags "${flags} -fsanitize=address" )
  # Analyse only.
  # set( flags "${flags} --analyze -Qunused-arguments" )
elseif( ${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 4.5 )
  set( flags "-pipe -std=gnu++0x" )
elseif( ANDROID )
  set( flags "-pipe -std=gnu++0x -pedantic -fstrict-enums" )
else()
  set( flags "-pipe -std=c++0x -pedantic -fstrict-enums" )
endif()

set( flags "${flags} -fno-exceptions -fno-rtti -fvisibility-inlines-hidden -ffast-math" )

# Additional warnings.
if( ${CMAKE_CXX_COMPILER_ID} STREQUAL Clang )
  set( flags "${flags} -Weverything -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-gnu" )
  set( flags "${flags} -Wno-disabled-macro-expansion -Wno-shadow -Wno-padded -Wno-float-equal" )
  set( flags "${flags} -Wno-global-constructors -Wno-exit-time-destructors" )
else()
  set( flags "${flags} -Wall -Wextra -Wconversion -Wcast-align -Winit-self -Wlogical-op -Wundef" )
  set( flags "${flags} -Wmissing-declarations -Wnon-virtual-dtor -Woverloaded-virtual" )
  set( flags "${flags} -Wformat=2 -Wmissing-format-attribute -Winvalid-pch" )

  if( NOT ${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 4.6 )
    # set( flags "${flags} -Wsuggest-attribute=noreturn" )
  endif()
  if( NOT ${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 4.7 )
    # set( flags "${flags} -Wzero-as-null-pointer-constant" )
  endif()
endif()

set( CMAKE_CXX_FLAGS "${flags} ${CMAKE_CXX_FLAGS}" )

# Strings for BuildInfo class.
string( TOUPPER "CMAKE_CXX_FLAGS_${CMAKE_BUILD_TYPE}" OZ_CXX_FLAGS )
string( STRIP "${CMAKE_CXX_FLAGS} ${${OZ_CXX_FLAGS}}" OZ_CXX_FLAGS )

site_name( OZ_HOSTNAME )
mark_as_advanced( OZ_HOSTNAME )

#
# Android project
#

if( ANDROID )
  add_custom_command( OUTPUT ../Android
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/etc/android-project ../Android )
  add_custom_target( android_project ALL DEPENDS ${CMAKE_BINARY_DIR}/../Android )
endif()

#
# Sources
#

add_subdirectory( src )

#
# Documentation
#

add_subdirectory( doc )

#
# Data files
#

# READMEs, launchers and libraries for standalone packages
if( OZ_STANDALONE )
  file( GLOB readmes doc/*.html )
  file( GLOB dataFiles share/openzone/*.zip )

  install( FILES AUTHORS COPYING README.md ChangeLog.md TODO.md DESTINATION doc )
  install( FILES ${dataFiles} DESTINATION share/openzone )
  install( FILES ${readmes} DESTINATION . )
  install( DIRECTORY doc/licences DESTINATION . )

  if( WIN32 )
    file( GLOB winLaunchers etc/launchers/*.bat )

    install( PROGRAMS ${winLaunchers} DESTINATION . )
    install( DIRECTORY lib/${OZ_PLATFORM_NAME} DESTINATION bin USE_SOURCE_PERMISSIONS )
  else()
    install( PROGRAMS etc/launchers/openzone.sh DESTINATION . )
    install( DIRECTORY lib/${OZ_PLATFORM_NAME} DESTINATION lib USE_SOURCE_PERMISSIONS )
  endif()
endif()

#
# CPack
#

set( CPACK_GENERATOR ZIP )

set( CPACK_PACKAGE_VERSION ${OZ_VERSION} )
set( CPACK_PACKAGE_DESCRIPTION_SUMMARY "A simple cross-platform FPS/RTS game engine" )
set( CPACK_PACKAGE_VENDOR "Davorin Učakar" )
set( CPACK_PACKAGE_CONTACT "Davorin Učakar <davorin.ucakar@gmail.com>" )
set( CPACK_TOPLEVEL_TAG "openzone" )
set( CPACK_SYSTEM_NAME ${OZ_PLATFORM_NAME} )
set( CPACK_RESOURCE_FILE_LICENSE ${CMAKE_SOURCE_DIR}/COPYING )
set( CPACK_RESOURCE_FILE_README ${CMAKE_SOURCE_DIR}/README.md )
set( CPACK_PACKAGE_EXECUTABLES "bin/openzone" "OpenZone" ) # start menu entry
set( CPACK_STRIP_FILES ON )
set( CPACK_PACKAGE_INSTALL_DIRECTORY "openzone" )
set( CPACK_PACKAGE_INSTALL_REGISTRY_KEY "openzone" )

set( CPACK_NSIS_DISPLAY_NAME "OpenZone" )
set( CPACK_NSIS_EXTRA_INSTALL_COMMANDS "WriteRegStr HKLM \\\"SYSTEM\\\\CurrentControlSet\\\\"
     "Control\\\\Session Manager\\\\Environment\\\" \\\"SDL_STDIO_REDIRECT\\\" \\\"0\\\"" )
set( CPACK_NSIS_EXTRA_INSTALL_COMMANDS "${CPACK_NSIS_EXTRA_INSTALL_COMMANDS};"
     "ExecWait '\\\"$INSTDIR\\\\support\\\\oalinst.exe\\\"'" )
set( CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "DeleteRegValue HKLM \\\"SYSTEM\\\\CurrentControlSet\\\\"
     "Control\\\\Session Manager\\\\Environment\\\" \\\"SDL_STDIO_REDIRECT\\\"" )
