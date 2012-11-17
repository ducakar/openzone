cmake_minimum_required( VERSION 2.8 )

project( openzone C CXX )

#
# Configuration
#

set( OZ_VERSION 0.3.80 )

option( OZ_SHARED_LIBS "Build libozCore and libozDynamics as shared libraries." OFF )
option( OZ_TRACK_ALLOCS "Track memory allocations to detect new/delete mismatches and memory leaks." OFF )
option( OZ_SIMD_MATH "Use SIMD implementation of linear algebra classes." OFF )

option( OZ_LUAJIT "Use use LuaJIT 2.0 instead of official Lua." OFF )
option( OZ_SDL2 "Use SDL 2.0 instead of SDL 1.2." OFF )
option( OZ_GL_ES "Use OpenGL ES 2.0 instead of OpenGL 2.1+." OFF )
option( OZ_NONFREE "Enable support for building S3TC textures." OFF )

option( OZ_STANDALONE "Install layout appropriate for launching from its own folder" OFF )

#
# Internal configuration
#

include( cmake/PCH.cmake )
include( CheckCXXSourceCompiles )
include( CheckCXXCompilerFlag )
include( CheckLibraryExists )

if( ANDROID )
  set( CMAKE_SYSTEM_NAME "Android" )
  set( OZ_SHARED_LIBS OFF )
  set( OZ_SDL2 ON )
  set( OZ_GL_ES ON )
  set( OZ_STANDALONE OFF )
elseif( NACL )
  set( CMAKE_SYSTEM_NAME "NaCl" )
  set( OZ_SHARED_LIBS OFF )
  set( OZ_SDL2 OFF )
  set( OZ_GL_ES ON )
  set( OZ_STANDALONE OFF )
elseif( WIN32 )
  set( OZ_STANDALONE ON )
endif()

set( OZ_PLATFORM_NAME "${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}" )
set( BUILD_SHARED_LIBS ${OZ_SHARED_LIBS} )
if( OZ_STANDALONE )
  set( CMAKE_INSTALL_PREFIX "/OpenZone-${OZ_VERSION}" )
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

if( ANDROID )
  include_directories( ${PLATFORM_PREFIX}/usr/include ) # For KDevelop.
  include_directories( ${PLATFORM_PORTS_PREFIX}/usr/include )

  find_package( ZLIB REQUIRED )
  find_package( PhysFS REQUIRED )

  find_package( Lua51 )
  find_path( LUAJIT_INCLUDE_DIR luajit.h PATH_SUFFIXES include/luajit-2.0 )
  find_library( LUAJIT_LIBRARY luajit-5.1 )

  find_path( SDL2_INCLUDE_DIR SDL.h PATH_SUFFIXES include/SDL2 )
  find_library( SDL2_LIBRARY SDL2 )
  find_library( SDL2_TTF_LIBRARY SDL2_ttf )
  find_package( Freetype REQUIRED )
  find_package( OpenAL REQUIRED )
  find_library( OGG_LIBRARY ogg )
  find_library( VORBIS_LIBRARY vorbis )
  find_library( VORBISFILE_LIBRARY vorbisfile )
elseif( NACL )
  include_directories( ${PLATFORM_PREFIX}/include ) # For KDevelop.
  include_directories( ${PLATFORM_PORTS_PREFIX}/usr/include )

  find_package( ZLIB REQUIRED )
  find_package( PhysFS REQUIRED )

  find_package( Lua51 )
  find_path( LUAJIT_INCLUDE_DIR luajit.h PATH_SUFFIXES include/luajit-2.0 )
  find_library( LUAJIT_LIBRARY luajit-5.1 )

  find_package( SDL REQUIRED )
  find_package( SDL_ttf REQUIRED )
  find_package( Freetype REQUIRED )
  find_package( OpenAL REQUIRED )
  find_library( OGG_LIBRARY ogg )
  find_library( VORBIS_LIBRARY vorbis )
  find_library( VORBISFILE_LIBRARY vorbisfile )
elseif( WIN32 )
  find_package( PhysFS REQUIRED )

  find_package( Lua51 )
  find_path( LUAJIT_INCLUDE_DIR luajit.h PATH_SUFFIXES include/luajit-2.0 )
  find_library( LUAJIT_LIBRARY luajit-5.1 )

  find_package( SDL )
  find_package( SDL_ttf )
  find_path( SDL2_INCLUDE_DIR SDL.h PATH_SUFFIXES include/SDL2 )
  find_library( SDL2_LIBRARY SDL2 )
  find_library( SDL2_TTF_LIBRARY SDL2_ttf )
  find_package( OpenGL REQUIRED )
  find_package( OpenAL REQUIRED )
  find_library( VORBISFILE_LIBRARY vorbisfile )

  find_library( FREEIMAGE_LIBRARY freeimage )
  find_library( SQUISH_LIBRARY squish )
else()
  find_package( PhysFS REQUIRED )

  find_package( Lua51 )
  find_path( LUAJIT_INCLUDE_DIR luajit.h PATH_SUFFIXES include/luajit-2.0 )
  find_library( LUAJIT_LIBRARY luajit-5.1 )

  find_package( SDL )
  find_package( SDL_ttf )
  find_path( SDL2_INCLUDE_DIR SDL.h PATH_SUFFIXES include/SDL2 )
  find_library( SDL2_LIBRARY SDL2 )
  find_library( SDL2_TTF_LIBRARY SDL2_ttf )
  find_package( OpenGL REQUIRED )
  find_package( OpenAL REQUIRED )
  find_library( VORBISFILE_LIBRARY vorbisfile )

  find_library( FREEIMAGE_LIBRARY freeimage )
  find_library( SQUISH_LIBRARY squish )
endif()

mark_as_advanced( MINGW32_LIBRARY )
mark_as_advanced( PHYSFS_INCLUDE_DIR )
mark_as_advanced( PHYSFS_LIBRARY )

mark_as_advanced( LUAJIT_INCLUDE_DIR )
mark_as_advanced( LUAJIT_LIBRARY )
mark_as_advanced( SDLMAIN_LIBRARY )
mark_as_advanced( SDL_INCLUDE_DIR )
mark_as_advanced( SDL_LIBRARY )
mark_as_advanced( SDL2_INCLUDE_DIR )
mark_as_advanced( SDL2_LIBRARY )
mark_as_advanced( SDL2_TTF_LIBRARY )
mark_as_advanced( GLES2_LIBRARY )
mark_as_advanced( OGG_LIBRARY )
mark_as_advanced( VORBIS_LIBRARY )
mark_as_advanced( VORBISFILE_LIBRARY )

mark_as_advanced( FREEIMAGE_LIBRARY )
mark_as_advanced( SQUISH_LIBRARY )

if( OZ_LUAJIT )
  include_directories( SYSTEM ${LUAJIT_INCLUDE_DIR} )
endif()
if( OZ_SDL2 )
  include_directories( SYSTEM ${SDL2_INCLUDE_DIR} )
else()
  include_directories( SYSTEM ${SDL_INCLUDE_DIR} )
endif()

# Local libmad, faad and eSpeak headers.
include_directories( SYSTEM ./include )

#
# Compiler flags
#

if( ${CMAKE_CXX_COMPILER_ID} STREQUAL Clang )
  set( flags "-std=c++11 -pedantic" )

  # AddressSanitizer memory checker
  # set( flags "${flags} -faddress-sanitizer" )
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

  install( FILES ${readmes} DESTINATION . )
  install( DIRECTORY doc/licences DESTINATION . )

  file( GLOB dataFiles share/openzone/*.zip )
  install( FILES ${dataFiles} DESTINATION share/openzone )

  if( WIN32 )
    install( DIRECTORY libs/${OZ_PLATFORM_NAME}/ DESTINATION bin/${OZ_PLATFORM_NAME}
             USE_SOURCE_PERMISSIONS )
    install( PROGRAMS libs/oalinst.exe DESTINATION . )
  else()
    install( DIRECTORY libs/${OZ_PLATFORM_NAME}/ DESTINATION lib/${OZ_PLATFORM_NAME}
             USE_SOURCE_PERMISSIONS )
  endif()

  if( WIN32 )
    file( GLOB launchers etc/launchers/*.bat )
    install( PROGRAMS ${launchers} DESTINATION . )
  else()
    install( PROGRAMS etc/launchers/openzone.sh DESTINATION . )
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
