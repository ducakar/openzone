cmake_minimum_required( VERSION 2.8 )

project( openzone C CXX )

#
# Configuration
#

set( OZ_VERSION "0.2.90" CACHE STRING "" FORCE )

option( OZ_SHARED_LIBOZ "Build liboz as a shared library." OFF )
option( OZ_TRACK_LEAKS "Save stack trace on operator new calls to track memory leaks." OFF )

set( OZ_APPLICATION_NAME "openzone" )
set( OZ_APPLICATION_TITLE "OpenZone" )

option( OZ_GL_ES "Disable features incompatible with OpenGL ES 2.0 and Mesa < 8.0." ON )
option( OZ_NONFREE "Enable support for MP3 and AAC." OFF )

option( OZ_STANDALONE "Install layout appropriate for launching from its own folder" OFF )

#
# Internal configuration
#

include( cmake/pch.cmake )
include( CheckCXXSourceCompiles )
include( CheckCXXCompilerFlag )
include( CheckLibraryExists )

if( NOT CMAKE_BUILD_TYPE STREQUAL "Debug" AND
    NOT CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo" AND
    NOT CMAKE_BUILD_TYPE STREQUAL "Release" AND
    NOT CMAKE_BUILD_TYPE STREQUAL "RelGenProf" AND
    NOT CMAKE_BUILD_TYPE STREQUAL "RelUseProf" )
  message( FATAL_ERROR "CMAKE_BUILT_TYPE must be either Debug, RelWithDebInfo, Release, RelGenProf "
           "or RelUseProf." )
endif()

if( NACL )
  set( CMAKE_SYSTEM_NAME "NaCl" )

  if( OZ_SHARED_LIBOZ )
    message( FATAL_ERROR "Shared libraries are not supported on NaCl/newlib." )
  endif()
  if( NOT OZ_GL_ES )
    message( FATAL_ERROR "Only OpenGL ES is supported on NaCl." )
  endif()
  if( OZ_NONFREE )
    message( FATAL_ERROR "Non-free music formats are not supported for NaCl." )
  endif()
endif()

if( WIN32 AND NOT CMAKE_INSTALL_PREFIX )
  set( CMAKE_INSTALL_PREFIX "../share/openzone" CACHE PATH "Path to data files" FORCE )
endif()

set( OZ_SYSTEM_NAME "${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}" CACHE STRING "" FORCE )

if( OZ_SHARED_LIBOZ )
  set( BUILD_SHARED_LIBS ON )
endif()

if( "${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "x86_64" )
  set( libSuffix "64" )
endif()

mark_as_advanced( OZ_VERSION )
mark_as_advanced( OZ_APPLICATION_NAME )
mark_as_advanced( OZ_APPLICATION_TITLE )
mark_as_advanced( OZ_SYSTEM_NAME )

#
# Libraries
#

if( NACL )
  find_library( PTHREAD_LIBRARY pthread )
  find_library( PEPPER_LIBRARY ppapi )
  find_library( PEPPER_CXX_LIBRARY ppapi_cpp )
  find_package( PhysFS REQUIRED )

  find_package( Lua51 REQUIRED )
  find_package( SDL REQUIRED )
  find_package( SDL_ttf REQUIRED )
  find_package( Freetype REQUIRED )
  find_package( ZLIB REQUIRED )
  find_library( PEPPER_GLES2_LIBRARY ppapi_gles2 )
  find_package( OpenAL REQUIRED )
  find_library( OGG_LIBRARY ogg )
  find_library( VORBIS_LIBRARY vorbis )
  find_library( VORBISFILE_LIBRARY vorbisfile )

  find_library( FREEIMAGE_LIBRARY freeimage )
elseif( WIN32 )
  find_library( WINMM_LIBRARY winmm )
  find_package( PhysFS REQUIRED )

  find_package( Lua51 REQUIRED )
  find_package( SDL REQUIRED )
  find_package( SDL_net REQUIRED )
  find_package( SDL_ttf REQUIRED )
  find_package( OpenGL REQUIRED )
  find_package( OpenAL REQUIRED )
  find_library( VORBISFILE_LIBRARY vorbisfile )
  find_library( MAD_LIBRARY mad )
  find_library( FAAD_LIBRARY faad )

  find_library( FREEIMAGE_LIBRARY freeimage )
else()
  find_library( PTHREAD_LIBRARY pthread )
  find_library( DL_LIBRARY dl )
  find_library( RT_LIBRARY rt )
  find_package( PhysFS REQUIRED )

  find_package( Lua51 REQUIRED )
  find_package( SDL REQUIRED )
  find_package( SDL_net REQUIRED )
  find_package( SDL_ttf REQUIRED )
  find_package( OpenGL REQUIRED )
  find_package( OpenAL REQUIRED )
  find_library( VORBISFILE_LIBRARY vorbisfile )

  find_library( FREEIMAGE_LIBRARY freeimage )
endif()

mark_as_advanced( PTHREAD_LIBRARY )
mark_as_advanced( DL_LIBRARY )
mark_as_advanced( RT_LIBRARY )
mark_as_advanced( MINGW32_LIBRARY )
mark_as_advanced( WINMM_LIBRARY )
mark_as_advanced( PEPPER_LIBRARY )
mark_as_advanced( PEPPER_CXX_LIBRARY )
mark_as_advanced( PHYSFS_INCLUDE_DIR )
mark_as_advanced( PHYSFS_LIBRARY )

mark_as_advanced( SDL_INCLUDE_DIR )
mark_as_advanced( SDL_LIBRARY )
mark_as_advanced( SDLMAIN_LIBRARY )
mark_as_advanced( SDLNET_INCLUDE_DIR )
mark_as_advanced( SDLNET_LIBRARY )
mark_as_advanced( SDLTTF_INCLUDE_DIR )
mark_as_advanced( SDLTTF_LIBRARY )
mark_as_advanced( PEPPER_GLES2_LIBRARY )
mark_as_advanced( OGG_LIBRARY )
mark_as_advanced( VORBIS_LIBRARY )
mark_as_advanced( VORBISFILE_LIBRARY )
mark_as_advanced( MAD_LIBRARY )
mark_as_advanced( FAAD_LIBRARY )

mark_as_advanced( FREEIMAGE_LIBRARY )

if( WIN32 )
  include_directories( ./include )
endif()

include_directories( "${SDL_INCLUDE_DIR}" )

if( NACL )
  set( libs_oz "${PTHREAD_LIBRARY};${PHYSFS_LIBRARY};${PEPPER_CXX_LIBRARY};${PEPPER_LIBRARY}" )
elseif( ANDROID )
  set( libs_oz "${PTHREAD_LIBRARY};${PHYSFS_LIBRARY}" )
elseif( WIN32 )
  set( libs_oz "${PTHREAD_LIBRARY};${WINMM_LIBRARY};${PHYSFS_LIBRARY}" )
else()
  set( libs_oz "${PTHREAD_LIBRARY};${DL_LIBRARY};${RT_LIBRARY};${PHYSFS_LIBRARY}" )
endif()

if( NACL )
  set( libs_client "${LUA_LIBRARY}" )
  set( libs_client "${libs_client};${SDL_LIBRARY}" )
  set( libs_client "${libs_client};${SDLTTF_LIBRARY}" )
  set( libs_client "${libs_client};${FREETYPE_LIBRARY}" )
  set( libs_client "${libs_client};${ZLIB_LIBRARY}" )
  set( libs_client "${libs_client};${PEPPER_GLES2_LIBRARY}" )
  set( libs_client "${libs_client};${OPENAL_LIBRARY}" )
  set( libs_client "${libs_client};${VORBISFILE_LIBRARY}" )
  set( libs_client "${libs_client};${VORBIS_LIBRARY}" )
  set( libs_client "${libs_client};${OGG_LIBRARY}" )
else()
  set( libs_client "${LUA_LIBRARY}" )
  set( libs_client "${libs_client};${SDL_LIBRARY}" )
  set( libs_client "${libs_client};${SDLNET_LIBRARY}" )
  set( libs_client "${libs_client};${SDLTTF_LIBRARY}" )
  set( libs_client "${libs_client};${OPENGL_gl_LIBRARY}" )
  set( libs_client "${libs_client};${OPENAL_LIBRARY}" )
  set( libs_client "${libs_client};${VORBISFILE_LIBRARY}" )

  if( WIN32 )
    set( libs_client "${libs_client};${MAD_LIBRARY}" )
    set( libs_client "${libs_client};${FAAD_LIBRARY}" )
  endif()

  set( libs_tools "${libs_client};${FREEIMAGE_LIBRARY}" )
endif()

#
# Compiler flags
#

set( flags "-pipe -std=c++0x -pedantic -fstrict-enums -ffast-math -msse3 -mfpmath=sse" )
set( warnings "-Wall -Wextra -Winvalid-pch -Wconversion " )

# Additional warnings, enabled only once in a while to check code quality (GCC specific).
# set( warnings "${warnings} -Wmissing-declarations -Wmissing-format-attribute" )
# set( warnings "${warnings} -Wnon-virtual-dtor -Woverloaded-virtual -Wcast-align -Wundef" )
# set( warnings "${warnings} -Winit-self -Wformat=2 -Wlogical-op" )
# GCC >= 4.7
# set( warnings "${warnings} -Wzero-as-null-pointer-constant" )

# Additional warnings (Clang specific).
# set( warnings "${warnings} -Weverything -Wno-c++98-compat -Wno-c++98-compat-pedantic" )
# set( warnings "${warnings} -Wno-disabled-macro-expansion -Wno-exit-time-destructors" )
# set( warnings "${warnings} -Wno-float-equal -Wno-global-constructors -Wno-padded -Wno-shadow " )

check_cxx_source_compiles( "
#ifdef __clang__
int main( int, char** ) { return 0; }
#else
# error Not LLVM/Clang
#endif
" isClang )

check_cxx_source_compiles( "
#if defined( __GNUC__ ) && __GNUC__ == 4 && __GNUC_MINOR__ <= 4
int main( int, char** ) { return 0; }
#else
# error GCC >= 4.5
#endif
" isLegacyGCC )

if( isClang )
  set( flags "-Qunused-arguments ${flags}" )

  # libc++
  set( flags "${flags} -stdlib=libc++" )
  set( libs_oz "${libs_oz};c++abi" )
  # libstdc++
  # set( flags "${flags} -stdlib=libstdc++" )
elseif( isLegacyGCC )
  set( flags "-pipe -std=gnu++0x -msse3 -ffast-math" )
endif()
if( ANDROID )
  set( flags "${flags} -fexceptions" )
endif()

check_cxx_compiler_flag( "-flto" hasLTO )
if( hasLTO )
  set( lto "-flto" )
endif()

set( CMAKE_CXX_FLAGS "${flags} ${warnings} ${CMAKE_CXX_FLAGS}" )
set( CMAKE_CXX_FLAGS_DEBUG "-D_FORTIFY_SOURCE=2 -g3 -O0" CACHE STRING "" FORCE )
set( CMAKE_CXX_FLAGS_RELWITHDEBINFO "-D_FORTIFY_SOURCE=2 -g3 -O3" CACHE STRING "" FORCE )
set( CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG -O3 ${lto}" CACHE STRING "" FORCE )
set( CMAKE_CXX_FLAGS_RELGENPROF "-DNDEBUG -O3 ${lto} -fprofile-generate" CACHE STRING "" FORCE )
set( CMAKE_CXX_FLAGS_RELUSEPROF "-DNDEBUG -O3 ${lto} -fprofile-use" CACHE STRING "" FORCE )

mark_as_advanced( CMAKE_CXX_FLAGS_RELGENPROF )
mark_as_advanced( CMAKE_CXX_FLAGS_RELUSEPROF )

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
install( DIRECTORY share/applications share/pixmaps DESTINATION share )

# info files and documentation
if( WIN32 OR OZ_STANDALONE )
  file( GLOB readmes "doc/*.html" )

  install( FILES AUTHORS COPYING ChangeLog BUGS TODO DESTINATION . )
  install( FILES ${readmes} DESTINATION doc )
  install( DIRECTORY doc/licences DESTINATION doc )
  install( DIRECTORY doc/doxygen/html/ DESTINATION doc/doxygen )
endif()

# libraries and launchers for standalone package
if( OZ_STANDALONE )
  file( GLOB dataFiles "share/openzone/*.zip" )

  install( FILES ${dataFiles} DESTINATION share/openzone )

  if( WIN32 )
    install( DIRECTORY support/${OZ_SYSTEM_NAME}/ DESTINATION bin )
  else()
    install( DIRECTORY support/${OZ_SYSTEM_NAME}/ DESTINATION lib${libSuffix} )
  endif()

  if( WIN32 )
    install( FILES etc/launchers/openzone.bat DESTINATION . )
    install( FILES etc/launchers/ozBuild.bat DESTINATION . )
  else()
    install( FILES etc/launchers/openzone.sh DESTINATION . )
    install( FILES etc/launchers/ozBuild.sh DESTINATION . )
  endif()
endif()

#
# CPack
#

set( CPACK_GENERATOR "ZIP" )

set( CPACK_PACKAGE_VERSION "${OZ_VERSION}" )
set( CPACK_PACKAGE_DESCRIPTION_SUMMARY "A simple cross-platform FPS/RTS game engine" )
set( CPACK_PACKAGE_VENDOR "Davorin Učakar" )
set( CPACK_PACKAGE_CONTACT "Davorin Učakar <davorin.ucakar@gmail.com>" )
set( CPACK_TOPLEVEL_TAG "${OZ_APPLICATION_NAME}" )
set( CPACK_SYSTEM_NAME "${OZ_SYSTEM_NAME}" )
set( CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/COPYING" )
set( CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/doc/README.html" )
set( CPACK_PACKAGE_EXECUTABLES "bin/openzone" "OpenZone" ) # start menu entry
set( CPACK_STRIP_FILES "bin/openzone;bin/ozBuild" )
set( CPACK_PACKAGE_INSTALL_DIRECTORY "${OZ_APPLICATION_NAME}" )
set( CPACK_PACKAGE_INSTALL_REGISTRY_KEY "${OZ_APPLICATION_NAME}" )

set( CPACK_NSIS_DISPLAY_NAME "${OZ_APPLICATION_TITLE}" )
set( CPACK_NSIS_EXTRA_INSTALL_COMMANDS "WriteRegStr HKLM \\\"SYSTEM\\\\CurrentControlSet\\\\"
     "Control\\\\Session Manager\\\\Environment\\\" \\\"SDL_STDIO_REDIRECT\\\" \\\"0\\\"" )
set( CPACK_NSIS_EXTRA_INSTALL_COMMANDS "${CPACK_NSIS_EXTRA_INSTALL_COMMANDS};"
     "ExecWait '\\\"$INSTDIR\\\\support\\\\oalinst.exe\\\"'" )
set( CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "DeleteRegValue HKLM \\\"SYSTEM\\\\CurrentControlSet\\\\"
     "Control\\\\Session Manager\\\\Environment\\\" \\\"SDL_STDIO_REDIRECT\\\"" )

include( CPack )
