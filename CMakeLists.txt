cmake_minimum_required( VERSION 2.8 )

project( openzone C CXX )

#
# Configuration
#

set( OZ_VERSION "0.3.80" )

option( OZ_SHARED_LIBS "Build liboz and libozdyn as shared libraries." OFF )
option( OZ_TRACK_ALLOCS "Track memory allocations to detect new/delete mismatches and memory leaks." OFF )

option( OZ_SIMD_MATH "Use SIMD implementation of linear algebra classes." OFF )
option( OZ_GL_ES "Disable features incompatible with OpenGL ES 2.0 and Mesa < 8.0." OFF )
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
  set( OZ_GL_ES ON )
  set( OZ_STANDALONE OFF )
elseif( NACL )
  set( CMAKE_SYSTEM_NAME "NaCl" )
  set( OZ_SHARED_LIBS OFF )
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

check_cxx_source_compiles( "
#ifdef __clang__
int main( int, char** ) { return 0; }
#else
# error Not LLVM/Clang
#endif
" isClang )

check_cxx_source_compiles( "
#if !defined( __clang__ ) && defined( __GNUC__ ) && __GNUC__ == 4 && __GNUC_MINOR__ <= 4
int main( int, char** ) { return 0; }
#else
# error GCC >= 4.5
#endif
" isLegacyGCC )

check_cxx_compiler_flag( "-flto" hasLTO )

#
# Libraries
#

if( ANDROID )
  include_directories( ${PLATFORM_PREFIX}/usr/include ) # For KDevelop
  include_directories( ${PLATFORM_PORTS_PREFIX}/usr/include )

  find_library( ANDROID_LOG_LIBRARY log )
  find_library( OPENSLES_LIBRARY OpenSLES )
  find_package( ZLIB REQUIRED )
  find_package( PhysFS REQUIRED )

  find_package( Lua51 REQUIRED )
  set( SDL_INCLUDE_DIR ${PLATFORM_PORTS_PREFIX}/usr/include/SDL2 )
  set( SDL_LIBRARY ${PLATFORM_PORTS_PREFIX}/usr/lib/libSDL2.a )
  set( SDLTTF_INCLUDE_DIR ${PLATFORM_PORTS_PREFIX}/usr/include/SDL2 )
  set( SDLTTF_LIBRARY ${PLATFORM_PORTS_PREFIX}/usr/lib/libSDL2_ttf.a )
  find_package( Freetype REQUIRED )
  find_library( ANDROID_GLES2_LIBRARY GLESv2 )
  find_package( OpenAL REQUIRED )
  find_library( OGG_LIBRARY ogg )
  find_library( VORBIS_LIBRARY vorbis )
  find_library( VORBISFILE_LIBRARY vorbisfile )
elseif( NACL )
  include_directories( ${PLATFORM_PREFIX}/include ) # For KDevelop
  include_directories( ${PLATFORM_PORTS_PREFIX}/usr/include )

  find_library( PTHREAD_LIBRARY pthread )
  find_library( PEPPER_LIBRARY ppapi )
  find_library( PEPPER_CXX_LIBRARY ppapi_cpp )
  find_package( ZLIB REQUIRED )
  find_package( PhysFS REQUIRED )

  find_package( Lua51 REQUIRED )
  find_package( SDL REQUIRED )
  find_package( SDL_ttf REQUIRED )
  find_package( Freetype REQUIRED )
  find_library( PEPPER_GLES2_LIBRARY ppapi_gles2 )
  find_package( OpenAL REQUIRED )
  find_library( OGG_LIBRARY ogg )
  find_library( VORBIS_LIBRARY vorbis )
  find_library( VORBISFILE_LIBRARY vorbisfile )
elseif( WIN32 )
  find_library( WINMM_LIBRARY winmm )
  find_package( PhysFS REQUIRED )

  find_package( Lua51 REQUIRED )
  find_package( SDL REQUIRED )
  find_package( SDL_ttf REQUIRED )
  find_package( OpenGL REQUIRED )
  find_package( OpenAL REQUIRED )
  find_library( VORBISFILE_LIBRARY vorbisfile )

  find_library( FREEIMAGE_LIBRARY freeimage )
  find_library( SQUISH_LIBRARY squish )
else()
  find_library( PTHREAD_LIBRARY pthread )
  find_library( RT_LIBRARY rt )
  find_library( DL_LIBRARY dl )
  find_package( ALSA )
  find_package( PhysFS REQUIRED )

  find_package( Lua51 REQUIRED )
  find_package( SDL REQUIRED )
  find_package( SDL_ttf REQUIRED )
  find_package( OpenGL REQUIRED )
  find_package( OpenAL REQUIRED )
  find_library( VORBISFILE_LIBRARY vorbisfile )

  find_library( FREEIMAGE_LIBRARY freeimage )
  find_library( SQUISH_LIBRARY squish )
endif()

mark_as_advanced( PTHREAD_LIBRARY )
mark_as_advanced( RT_LIBRARY )
mark_as_advanced( DL_LIBRARY )
mark_as_advanced( MINGW32_LIBRARY )
mark_as_advanced( WINMM_LIBRARY )
mark_as_advanced( PEPPER_LIBRARY )
mark_as_advanced( PEPPER_CXX_LIBRARY )
mark_as_advanced( ANDROID_LOG_LIBRARY )
mark_as_advanced( OPENSLES_LIBRARY )
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
mark_as_advanced( ANDROID_GLES2_LIBRARY )
mark_as_advanced( OGG_LIBRARY )
mark_as_advanced( VORBIS_LIBRARY )
mark_as_advanced( VORBISFILE_LIBRARY )

mark_as_advanced( FREEIMAGE_LIBRARY )
mark_as_advanced( SQUISH_LIBRARY )

include_directories( ${SDL_INCLUDE_DIR} )
if( ANDROID OR NACL OR WIN32 )
  include_directories( ./include )
endif()

if( ANDROID )
  set( libs_oz ${PLATFORM_STLLIB} )
  set( libs_oz ${libs_oz};${ANDROID_LOG_LIBRARY} )
  set( libs_oz ${libs_oz};${OPENSLES_LIBRARY} )
  set( libs_oz ${libs_oz};${PHYSFS_LIBRARY} )
  set( libs_oz ${libs_oz};${ZLIB_LIBRARY} )
elseif( NACL )
  set( libs_oz ${PTHREAD_LIBRARY} )
  set( libs_oz ${libs_oz};${PEPPER_CXX_LIBRARY} )
  set( libs_oz ${libs_oz};${PEPPER_LIBRARY} )
  set( libs_oz ${libs_oz};${PHYSFS_LIBRARY} )
  set( libs_oz ${libs_oz};${ZLIB_LIBRARY} )
elseif( WIN32 )
  set( libs_oz ${WINMM_LIBRARY} )
  set( libs_oz ${libs_oz};${PHYSFS_LIBRARY} )
else()
  set( libs_oz ${PTHREAD_LIBRARY} )
  set( libs_oz ${libs_oz};${RT_LIBRARY} )
  set( libs_oz ${libs_oz};${DL_LIBRARY} )
  set( libs_oz ${libs_oz};${ALSA_LIBRARY} )
  set( libs_oz ${libs_oz};${PHYSFS_LIBRARY} )
endif()

if( ANDROID )
  set( libs_client ${LUA_LIBRARY} )
  set( libs_client ${libs_client};${SDL_LIBRARY} )
  set( libs_client ${libs_client};${SDLTTF_LIBRARY} )
  set( libs_client ${libs_client};${FREETYPE_LIBRARY} )
  set( libs_client ${libs_client};${ANDROID_GLES2_LIBRARY} )
  set( libs_client ${libs_client};${OPENAL_LIBRARY} )
  set( libs_client ${libs_client};${VORBISFILE_LIBRARY} )
  set( libs_client ${libs_client};${VORBIS_LIBRARY} )
  set( libs_client ${libs_client};${OGG_LIBRARY} )
elseif( NACL )
  set( libs_client ${LUA_LIBRARY} )
  set( libs_client ${libs_client};${SDL_LIBRARY} )
  set( libs_client ${libs_client};${SDLTTF_LIBRARY} )
  set( libs_client ${libs_client};${FREETYPE_LIBRARY} )
  set( libs_client ${libs_client};${PEPPER_GLES2_LIBRARY} )
  set( libs_client ${libs_client};${OPENAL_LIBRARY} )
  set( libs_client ${libs_client};${VORBISFILE_LIBRARY} )
  set( libs_client ${libs_client};${VORBIS_LIBRARY} )
  set( libs_client ${libs_client};${OGG_LIBRARY} )
else()
  set( libs_client ${LUA_LIBRARY} )
  set( libs_client ${libs_client};${SDL_LIBRARY} )
  set( libs_client ${libs_client};${SDLTTF_LIBRARY} )
  set( libs_client ${libs_client};${OPENGL_gl_LIBRARY} )
  set( libs_client ${libs_client};${OPENAL_LIBRARY} )
  set( libs_client ${libs_client};${VORBISFILE_LIBRARY} )

  set( libs_tools ${libs_client};${FREEIMAGE_LIBRARY} )
  if( OZ_NONFREE )
    set( libs_tools ${libs_tools};${SQUISH_LIBRARY} )
  endif()
endif()

#
# Compiler flags
#

if( isClang )
  set( flags "-std=c++11" )
  # AddressSanitizer memory checker
  # set( flags "${flags} -faddress-sanitizer" )
  # GNU C++ library
  # set( flags "${flags} -stdlib=libstdc++" )
  # LLVM C++ library
  # set( flags "${flags} -stdlib=libc++" )
  # set( libs_oz "c++abi;${libs_oz}" )
elseif( isLegacyGCC )
  set( flags "-pipe -std=gnu++0x" )
elseif( ANDROID OR WIN32 )
  set( flags "-pipe -std=gnu++0x -fstrict-enums" )
else()
  set( flags "-pipe -std=c++0x -fstrict-enums" )
endif()

set( flags "${flags} -fno-exceptions -fno-rtti -fvisibility-inlines-hidden -ffast-math" )
set( warnings "-Wall -Wextra -Wconversion" )

# Additional warnings.
if( isClang )
  set( warnings "${warnings} -Weverything -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-gnu" )
  set( warnings "${warnings} -Wno-disabled-macro-expansion -Wno-float-equal -Wno-padded" )
  set( warnings "${warnings} -Wno-global-constructors -Wno-exit-time-destructors -Wno-shadow" )
else()
  set( warnings "${warnings} -Winvalid-pch -Wmissing-declarations -Wmissing-format-attribute" )
  set( warnings "${warnings} -Wnon-virtual-dtor -Woverloaded-virtual -Wcast-align -Wundef" )
  set( warnings "${warnings} -Winit-self -Wformat=2 -Wlogical-op -Wabi" )
  # GCC >= 4.7
  # set( warnings "${warnings} -Wsuggest-attribute=noreturn -Wzero-as-null-pointer-constant" )
endif()

set( CMAKE_CXX_FLAGS "${flags} ${warnings} ${CMAKE_CXX_FLAGS}" )

if( hasLTO AND NOT WIN32 )
  # On Win32+LTO glCompileShaders() sets shaderFiles.data to null in loop at Shader.cc:394. WTF???
  # set( CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -flto" )
endif()

# construct strings for BuildInfo
string( TOUPPER "CMAKE_CXX_FLAGS_${CMAKE_BUILD_TYPE}" OZ_CXX_FLAGS )
string( TOUPPER "CMAKE_EXE_LINKER_FLAGS_${CMAKE_BUILD_TYPE}" OZ_EXE_LINKER_FLAGS )

set( libsString "" )
foreach( lib ${libs_client} )
  set( libsString "${libsString} ${lib}" )
endforeach()

set( OZ_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${${OZ_CXX_FLAGS}}" )
set( OZ_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${${OZ_EXE_LINKER_FLAGS}} ${libsString}" )

string( STRIP ${OZ_CXX_FLAGS} OZ_CXX_FLAGS )
string( STRIP ${OZ_EXE_LINKER_FLAGS} OZ_EXE_LINKER_FLAGS )

site_name( OZ_HOSTNAME )
mark_as_advanced( OZ_HOSTNAME )

#
# Android project
#

if( ANDROID )
  add_custom_command( OUTPUT ../Android
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/etc/android-project ../Android )
  add_custom_target( android_project ALL DEPENDS ../Android )
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

# icons
if( NOT OZ_STANDALONE )
  install( DIRECTORY share/applications share/pixmaps DESTINATION share )
endif()

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

set( CPACK_GENERATOR "ZIP" )

set( CPACK_PACKAGE_VERSION ${OZ_VERSION} )
set( CPACK_PACKAGE_DESCRIPTION_SUMMARY "A simple cross-platform FPS/RTS game engine" )
set( CPACK_PACKAGE_VENDOR "Davorin Učakar" )
set( CPACK_PACKAGE_CONTACT "Davorin Učakar <davorin.ucakar@gmail.com>" )
set( CPACK_TOPLEVEL_TAG "openzone" )
set( CPACK_SYSTEM_NAME ${OZ_PLATFORM_NAME} )
set( CPACK_RESOURCE_FILE_LICENSE ${CMAKE_SOURCE_DIR}/COPYING )
set( CPACK_RESOURCE_FILE_README ${CMAKE_SOURCE_DIR}/doc/README.html )
set( CPACK_PACKAGE_EXECUTABLES "bin/openzone" "OpenZone" ) # start menu entry
set( CPACK_STRIP_FILES "bin/openzone;bin/ozBuild" )
set( CPACK_PACKAGE_INSTALL_DIRECTORY "openzone" )
set( CPACK_PACKAGE_INSTALL_REGISTRY_KEY "openzone" )

set( CPACK_NSIS_DISPLAY_NAME "OpenZone" )
set( CPACK_NSIS_EXTRA_INSTALL_COMMANDS "WriteRegStr HKLM \\\"SYSTEM\\\\CurrentControlSet\\\\"
     "Control\\\\Session Manager\\\\Environment\\\" \\\"SDL_STDIO_REDIRECT\\\" \\\"0\\\"" )
set( CPACK_NSIS_EXTRA_INSTALL_COMMANDS "${CPACK_NSIS_EXTRA_INSTALL_COMMANDS};"
     "ExecWait '\\\"$INSTDIR\\\\support\\\\oalinst.exe\\\"'" )
set( CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "DeleteRegValue HKLM \\\"SYSTEM\\\\CurrentControlSet\\\\"
     "Control\\\\Session Manager\\\\Environment\\\" \\\"SDL_STDIO_REDIRECT\\\"" )

include( CPack )
