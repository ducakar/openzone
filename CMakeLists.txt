cmake_minimum_required( VERSION 2.8 )

project( openzone C CXX )

#
# Configuration
#

set( OZ_VERSION "0.2.85" CACHE STRING "" FORCE )
set( OZ_SOVERSION "0" CACHE STRING "" FORCE )

option( OZ_TRACK_LEAKS "Save stack trace on operator new calls to track memory leaks." OFF )

set( OZ_APPLICATION_NAME "openzone" CACHE STRING "" FORCE )
set( OZ_APPLICATION_TITLE "OpenZone" CACHE STRING "" FORCE )

option( OZ_NETWORKING "Enable networking (not supported yet)." OFF )
option( OZ_BUMPMAP "Enable bumpmapping (not supported yet)." OFF )

option( OZ_STANDALONE "Install layout appropriate for launching from its own folder" OFF )

#
# Internal configuration
#

if( NOT CMAKE_BUILD_TYPE )
  set( CMAKE_BUILD_TYPE "Debug" CACHE STRING "Debug RelWithDebInfo Release ReleaseGen ReleaseUse" FORCE )
endif()

if( WIN32 )
  set( OZ_SYSTEM_NAME "${CMAKE_SYSTEM_NAME}-i686" CACHE STRING "" FORCE )
else()
  set( OZ_SYSTEM_NAME "${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}" CACHE STRING "" FORCE )
endif()

if( "${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "x86_64" )
  set( OZ_LIB_SUFFIX "64" )
endif()

mark_as_advanced( OZ_VERSION )
mark_as_advanced( OZ_SOVERSION )
mark_as_advanced( OZ_APPLICATION_NAME )
mark_as_advanced( OZ_APPLICATION_TITLE )
mark_as_advanced( OZ_SYSTEM_NAME )

# detect clang
string( REGEX MATCH "clang\\+\\+$" compiler_base "${CMAKE_CXX_COMPILER}" )
if( "${compiler_base}" STREQUAL "clang++" )
  set( OZ_CLANG ON )
endif()

#
# Libraries
#

include( cmake/pch.cmake )
include( CheckLibraryExists )

if( WIN32 )
  find_library( WINMM_LIBRARY winmm )

  mark_as_advanced( MINGW32_LIBRARY )
  mark_as_advanced( WINMM_LIBRARY )
else()
  find_library( RT_LIBRARY rt )
  find_library( PTHREAD_LIBRARY pthread )
  find_library( PULSESIMPLE_LIBRARY pulse-simple )

  mark_as_advanced( RT_LIBRARY )
  mark_as_advanced( PTHREAD_LIBRARY )
  mark_as_advanced( PULSESIMPLE_LIBRARY )
endif()

find_package( PhysFS REQUIRED )
find_package( SDL REQUIRED )
find_package( SDL_ttf REQUIRED )
find_package( Lua51 REQUIRED )
find_package( OpenGL REQUIRED )
find_package( OpenAL REQUIRED )

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
if( NOT DEFINED MAD_INCLUDE_DIR )
  set( MAD_INCLUDE_DIR "" CACHE PATH "" FORCE )
endif()
if( NOT DEFINED FAAD_INCLUDE_DIR )
  set( FAAD_INCLUDE_DIR "" CACHE PATH "" FORCE )
endif()

find_library( FREEIMAGE_LIBRARY freeimage )
find_library( VORBISFILE_LIBRARY vorbisfile )

mark_as_advanced( PHYSFS_INCLUDE_DIR )
mark_as_advanced( PHYSFS_LIBRARY )
mark_as_advanced( SDL_INCLUDE_DIR )
mark_as_advanced( SDL_LIBRARY )
mark_as_advanced( SDLMAIN_LIBRARY )
mark_as_advanced( SDLTTF_INCLUDE_DIR )
mark_as_advanced( SDLTTF_LIBRARY )
mark_as_advanced( FREEIMAGE_INCLUDE_DIR )
mark_as_advanced( FREEIMAGE_LIBRARY )
mark_as_advanced( OGG_INCLUDE_DIR )
mark_as_advanced( VORBIS_INCLUDE_DIR )
mark_as_advanced( VORBISFILE_INCLUDE_DIR )
mark_as_advanced( VORBISFILE_LIBRARY )
mark_as_advanced( MAD_INCLUDE_DIR )
mark_as_advanced( FAAD_INCLUDE_DIR )

include_directories( ${PHYSFS_INCLUDE_DIR} )
include_directories( ${SDL_INCLUDE_DIR} )
include_directories( ${SDLTTF_INCLUDE_DIR} )
include_directories( ${LUA_INCLUDE_DIR} )
include_directories( ${OPENGL_INCLUDE_DIR} )
include_directories( ${OPENAL_INCLUDE_DIR} )
include_directories( ${FREEIMAGE_INCLUDE_DIR} )
include_directories( ${OGG_INCLUDE_DIR} )
include_directories( ${VORBIS_INCLUDE_DIR} )
include_directories( ${VORBISFILE_INCLUDE_DIR} )
include_directories( ${MAD_INCLUDE_DIR} )
include_directories( ${FAAD_INCLUDE_DIR} )

if( WIN32 )
  set( libs_oz "${WINMM_LIBRARY};${PHYSFS_LIBRARY}" )
else()
  set( libs_oz "${RT_LIBRARY};${PTHREAD_LIBRARY};${PULSESIMPLE_LIBRARY};${PHYSFS_LIBRARY}" )
endif()

set( libs_client "${PHYSFS_LIBRARY}" )
set( libs_client "${libs_client};${SDL_LIBRARY}" )
set( libs_client "${libs_client};${SDLTTF_LIBRARY}" )
set( libs_client "${libs_client};${LUA_LIBRARY}" )
set( libs_client "${libs_client};${OPENGL_gl_LIBRARY}" )
set( libs_client "${libs_client};${OPENAL_LIBRARY}" )
set( libs_client "${libs_client};${VORBISFILE_LIBRARY}" )

if( WIN32 )
  set( libs_client "${libs_client};${MAD_LIBRARY}" )
  set( libs_client "${libs_client};${FAAD_LIBRARY}" )
endif()

set( libs_tools "${libs_client};${FREEIMAGE_LIBRARY}" )

#
# Compiler flags
#

set( flags "-pipe -std=c++0x -pedantic -fstrict-enums -ffast-math -freciprocal-math" )
set( warnings "-Wall -Wextra -Wconversion -Woverloaded-virtual -Wnon-virtual-dtor -Winvalid-pch" )
set( arch_flags "-msse3 -mfpmath=sse" )

if( OZ_CLANG )
  set( flags "-Qunused-arguments ${flags}" )
endif()

set( CMAKE_BUILD_TYPE Debug CACHE STRING "Debug RelWithDebInfo Release ReleaseGen ReleaseUse" )

if( NOT WIN32 )
  set( lto "-flto" )
endif()

set( CMAKE_CXX_FLAGS "${flags} ${arch_flags} ${warnings} ${CMAKE_CXX_FLAGS}" )
set( CMAKE_CXX_FLAGS_DEBUG "-D_FORTIFY_SOURCE=2 -g3 -O0" CACHE STRING "" FORCE )
set( CMAKE_CXX_FLAGS_RELWITHDEBINFO "-D_FORTIFY_SOURCE=2 -g3 -O3" CACHE STRING "" FORCE )
set( CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG -O3 ${lto}" CACHE STRING "" FORCE )
set( CMAKE_CXX_FLAGS_RELEASEGEN "-DNDEBUG -O3 ${lto} -fprofile-generate" CACHE STRING "" FORCE )
set( CMAKE_CXX_FLAGS_RELEASEUSE "-DNDEBUG -O3 ${lto} -fprofile-use" CACHE STRING "" FORCE )

mark_as_advanced( CMAKE_CXX_FLAGS_RELEASEGEN )
mark_as_advanced( CMAKE_CXX_FLAGS_RELEASEUSE )
mark_as_advanced( CMAKE_EXE_LINKER_FLAGS_RELEASEGEN )
mark_as_advanced( CMAKE_EXE_LINKER_FLAGS_RELEASEUSE )

# construct flags strings for BuildInfo
string( TOUPPER "CMAKE_CXX_FLAGS_${CMAKE_BUILD_TYPE}" OZ_CXX_FLAGS )
string( TOUPPER "CMAKE_EXE_LINKER_FLAGS_${CMAKE_BUILD_TYPE}" OZ_EXE_LINKER_FLAGS )

set( libsString "" )
foreach( lib ${libs_client} )
  set( libsString "${libsString} ${lib}" )
endforeach()

set( OZ_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${${OZ_CXX_FLAGS}}" )
set( OZ_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${${OZ_EXE_LINKER_FLAGS}} ${libsString}" )

string( STRIP "${OZ_CXX_FLAGS}" OZ_CXX_FLAGS )
string( STRIP "${OZ_EXE_LINKER_FLAGS}" OZ_EXE_LINKER_FLAGS )

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

# icons
install( DIRECTORY share/applications share/pixmaps COMPONENT client DESTINATION share )

# info files and documentation
if( WIN32 OR OZ_STANDALONE )
  file( GLOB readmes doc/*.html )

  install( FILES AUTHORS COPYING ChangeLog BUGS TODO COMPONENT client DESTINATION . )
  install( FILES ${readmes} COMPONENT client DESTINATION doc )
  install( DIRECTORY doc/licences COMPONENT client DESTINATION doc )
  install( DIRECTORY doc/doxygen/html/ COMPONENT documentation DESTINATION doc/doxygen )
endif()

# libraries and launchers for standalone package
if( OZ_STANDALONE )
#   install( DIRECTORY support/${OZ_SYSTEM_NAME}-client/ COMPONENT client DESTINATION . )
#   install( DIRECTORY support/${OZ_SYSTEM_NAME}-tools/ COMPONENT tools DESTINATION . )

  if( WIN32 )
    install( FILES support/openzone.bat COMPONENT client DESTINATION . )
    install( FILES support/ozbuild.bat COMPONENT tools DESTINATION . )
  else()
    install( FILES support/openzone.sh COMPONENT client DESTINATION . )
    install( FILES support/ozbuild.sh COMPONENT tools DESTINATION . )
  endif()
endif()

#
# CPack
#

set( CPACK_GENERATOR "TGZ" )

set( CPACK_PACKAGE_VERSION "${OZ_VERSION}" )
set( CPACK_PACKAGE_DESCRIPTION_SUMMARY "A simple cross-platform FPS/RTS game engine" )
set( CPACK_PACKAGE_VENDOR "Davorin Učakar" )
set( CPACK_PACKAGE_CONTACT "Davorin Učakar <davorin.ucakar@gmail.com>" )
set( CPACK_TOPLEVEL_TAG "${OZ_APPLICATION_NAME}" )
set( CPACK_SYSTEM_NAME "${OZ_SYSTEM_NAME}" )
set( CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/COPYING" )
set( CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/doc/README.html" )
set( CPACK_PACKAGE_EXECUTABLES "bin/openzone" "OpenZone" ) # start menu entry
set( CPACK_STRIP_FILES "bin/openzone;bin/ozbuild" )
set( CPACK_PACKAGE_INSTALL_DIRECTORY "${OZ_APPLICATION_NAME}" )
set( CPACK_PACKAGE_INSTALL_REGISTRY_KEY "${OZ_APPLICATION_NAME}" )

set( CPACK_NSIS_DISPLAY_NAME "${OZ_APPLICATION_TITLE}" )
set( CPACK_NSIS_EXTRA_INSTALL_COMMANDS "WriteRegStr HKLM \\\"SYSTEM\\\\CurrentControlSet\\\\Control\\\\Session Manager\\\\Environment\\\" \\\"SDL_STDIO_REDIRECT\\\" \\\"0\\\"" )
set( CPACK_NSIS_EXTRA_INSTALL_COMMANDS "${CPACK_NSIS_EXTRA_INSTALL_COMMANDS};ExecWait '\\\"$INSTDIR\\\\support\\\\oalinst.exe\\\"'" )
set( CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "DeleteRegValue HKLM \\\"SYSTEM\\\\CurrentControlSet\\\\Control\\\\Session Manager\\\\Environment\\\" \\\"SDL_STDIO_REDIRECT\\\"" )

include( CPack )
