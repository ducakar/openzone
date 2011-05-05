cmake_minimum_required( VERSION 2.8 )

project( openzone C CXX )

option( OZ_TRACE_LEAKS "Use overloads for default new and delete that saves stacktrace for each new call and trace memory leaks" ON )
option( OZ_POOL_ALLOC "Enable Pool and new/delete overloads that use it. Otherwise use a dummy class that calls generic new/delete." ON )
option( OZ_BIG_ENDIAN_STREAM "Use big-endian byte order for binary files and networking" OFF )

if( NOT OZ_APPLICATION_NAME )
  set( OZ_APPLICATION_NAME "openzone" CACHE STRING "" FORCE )
endif()

if( NOT OZ_APPLICATION_VERSION )
  set( OZ_APPLICATION_VERSION "0.1.2" CACHE STRING "" FORCE )
endif()

if( NOT OZ_APPLICATION_TITLE )
  set( OZ_APPLICATION_TITLE "OpenZone" CACHE STRING "" FORCE )
endif()

if( NOT OZ_RC_DIR )
  if( WIN32 )
    set( OZ_RC_DIR "OpenZone" CACHE STRING "" FORCE )
  else()
    set( OZ_RC_DIR ".config/openzone" CACHE STRING "" FORCE )
  endif()
endif()

if( NOT OZ_CLIENT_LOG_FILE )
  set( OZ_CLIENT_LOG_FILE "STDOUT" CACHE STRING "" FORCE )
endif()

if( NOT OZ_CLIENT_CONFIG_FILE )
  set( OZ_CLIENT_CONFIG_FILE "client.rc" CACHE STRING "" FORCE )
endif()

option( OZ_CYLINDER "Enable cylinder collision model for object pairs that both have cylinder flag (in other cases AABB is used)." ON )
option( OZ_NETWORKING "Enable networking" OFF )
option( OZ_OPENGL3 "Compile for OpenGL 3+ instead for OpenGL 2.1" OFF )
option( OZ_TEXTURE_COMPRESSION "Use S3 texture compression (doesn't work with open source drivers)" OFF )
option( OZ_BUMPMAP "Enable bumpmap support (not implemented yet, only vertex format for now)" OFF )

option( OZ_BUILD_TESTS "Build tests" ON )
option( OZ_BUILD_TOOLS "Build tools" ON )
option( OZ_BUILD_CLIENT "Build client" ON )
option( OZ_BUILD_SERVER "Build server" OFF )

option( OZ_INSTALL_LIBOZ "Install liboz" ON )
option( OZ_INSTALL_CLIENT "Install openzone client executable" ON )
option( OZ_INSTALL_SERVER "Install openzone server executable" OFF )
option( OZ_INSTALL_DATA "Install game data" ON )

set( CPACK_PACKAGE_NAME "${OZ_APPLICATION_NAME}" )
set( CPACK_PACKAGE_VERSION "${OZ_APPLICATION_VERSION}" )
set( CPACK_PACKAGE_DESCRIPTION_SUMMARY "A simple cross-platform 3D game engine" )
set( CPACK_PACKAGE_CONTACT "Davorin Učakar <davorin.ucakar@gmail.com>" )
set( CPACK_PACKAGE_INSTALL_DIRECTORY "OpenZone" )
set( CPACK_PACKAGE_INSTALL_REGISTRY_KEY "OpenZone" )
set( CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/COPYING" )
set( CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README" )
if( NOT OZ_SYSTEM_NAME )
   set( OZ_SYSTEM_NAME "${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}" CACHE STRING "" FORCE )
endif()
set( CPACK_SYSTEM_NAME "${OZ_SYSTEM_NAME}" )
set( CPACK_PACKAGE_EXECUTABLES "openzone" "OpenZone" )
set( CPACK_STRIP_FILES "bin/openzone" )
set( CPACK_NSIS_DISPLAY_NAME "OpenZone" )
set( CPACK_COMPONENTS_ALL liboz client )
if( WIN32 )
  set( CPACK_NSIS_EXTRA_INSTALL_COMMANDS "WriteRegStr HKLM \\\"SYSTEM\\\\CurrentControlSet\\\\Control\\\\Session Manager\\\\Environment\\\" \\\"SDL_STDIO_REDIRECT\\\" \\\"0\\\"" )
  set( CPACK_NSIS_EXTRA_INSTALL_COMMANDS "${CPACK_NSIS_EXTRA_INSTALL_COMMANDS};ExecWait '\\\"$INSTDIR\\\\support\\\\oalinst.exe\\\"'" )
  set( CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "DeleteRegValue HKLM \\\"SYSTEM\\\\CurrentControlSet\\\\Control\\\\Session Manager\\\\Environment\\\" \\\"SDL_STDIO_REDIRECT\\\"" )
endif()
include( CheckLibraryExists )
include( CPack )

# add_pch( target_name header.hpp DEPENDS dep1 dep2 ... )
macro( add_pch _targetName _inputHeader )
  # extract CMAKE_CXX_FLAGS and CMAKE_CXX_FLAGS_XXX for current build configuration XXX
  string( TOUPPER "CMAKE_CXX_FLAGS_${CMAKE_BUILD_TYPE}" _build_type_flags_var )
  set( _flags "${CMAKE_CXX_FLAGS} ${${_build_type_flags_var}}" )

  # convert string of space separated flags into a list
  separate_arguments( _flags )

  # extract include directories set by include_directories command
  get_directory_property( _includes INCLUDE_DIRECTORIES )
  foreach( _inc_dir ${_includes} )
    list( APPEND _flags "-I${_inc_dir}" )
  endforeach()

  # set build rules (build precompiled header and copy original header to build folder - needed to
  # be there to be included via other headers - which is needed for KDevelop's indexer to work
  # properly)
  add_custom_command( OUTPUT "${_inputHeader}.gch"
    ${ARGN} "${_inputHeader}"
    COMMAND ${CMAKE_COMMAND} -E remove -f "${_inputHeader}"
    COMMAND ${CMAKE_COMMAND} -E remove -f "${_inputHeader}.gch"
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/${_inputHeader}" "${_inputHeader}"
    COMMAND ${CMAKE_CXX_COMPILER} ${_flags} -o "${_inputHeader}.gch" "${CMAKE_CURRENT_SOURCE_DIR}/${_inputHeader}" )
  add_custom_target( ${_targetName}
    SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/${_inputHeader}"
    DEPENDS "${_inputHeader}.gch" )
endmacro()

if( NOT CMAKE_BUILD_TYPE )
  set( CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type" FORCE )
endif()

check_library_exists( m sincosf "" OZ_HAVE_SINCOSF )

Find_Package( SDL REQUIRED )
Find_Package( SDL_ttf REQUIRED )
Find_Package( Lua51 REQUIRED )
Find_Package( OpenGL REQUIRED )
Find_Package( OpenAL REQUIRED )

if( OZ_NETWORKING )
  Find_Package( SDL_net REQUIRED )
endif()

if( OZ_BUILD_TOOLS )
  Find_Package( SDL_image REQUIRED )
  Find_Package( PNG REQUIRED )
  Find_Package( JPEG REQUIRED )
endif()

mark_as_advanced( SDL_INCLUDE_DIR )
mark_as_advanced( SDL_LIBRARY )
mark_as_advanced( SDLMAIN_LIBRARY )

mark_as_advanced( SDLNET_INCLUDE_DIR )
mark_as_advanced( SDLNET_LIBRARY )

mark_as_advanced( SDLIMAGE_INCLUDE_DIR )
mark_as_advanced( SDLIMAGE_LIBRARY )

mark_as_advanced( SDLTTF_INCLUDE_DIR )
mark_as_advanced( SDLTTF_LIBRARY )

if( WIN32 )
  find_library( INTL_LIBRARY intl )
endif()

if( NOT VORBISFILE_INCLUDE_DIR )
  set( VORBISFILE_INCLUDE_DIR "" CACHE PATH "" FORCE )
endif()

find_library( VORBISFILE_LIBRARY vorbisfile )

mark_as_advanced( VORBISFILE_INCLUDE_DIR )
mark_as_advanced( VORBISFILE_LIBRARY )

# detect clang
string( REGEX MATCH "clang\\+\\+" compiler_base "${CMAKE_CXX_COMPILER}" )
if( "${compiler_base}" STREQUAL "clang++" )
  set( OZ_CLANG ON )
endif()

# detect platform
if( WIN32 )
  set( OZ_MINGW ON )
endif()

include_directories( ${SDL_INCLUDE_DIR} )
include_directories( ${SDLTTF_INCLUDE_DIR} )
include_directories( ${LUA_INCLUDE_DIR} )
include_directories( ${OPENGL_INCLUDE_DIR} )
include_directories( ${OPENAL_INCLUDE_DIR} )
include_directories( ${OGG_INCLUDE_DIR} )
include_directories( ${VORBIS_INCLUDE_DIR} )

set( libs "${SDL_LIBRARY}" )
set( libs "${libs};${SDLTTF_LIBRARY}" )
set( libs "${libs};${LUA_LIBRARY}" )
set( libs "${libs};${OPENGL_gl_LIBRARY}" )
set( libs "${libs};${OPENAL_LIBRARY}" )
set( libs "${libs};${VORBISFILE_LIBRARY}" )

if( OZ_MINGW )
  set( libs "${libs};${INTL_LIBRARY}" )
endif()

if( OZ_NETWORK )
  include_directories( ${SDLNET_INCLUDE_DIR} )
  set( libs "${libs};${SDLNET_LIBRARY}" )
endif()

if( OZ_BUILD_TOOLS )
  include_directories( ${SDLIMAGE_INCLUDE_DIR} )
  set( libs "${libs};${SDLIMAGE_LIBRARY}" )
endif()

if( OZ_MINGW )
  include_directories( ./include )
endif()

# compiler and linker flags
set( flags "-pipe -std=gnu++0x -ffast-math" )
set( warnings "-Wall -Wextra -Wold-style-cast -Wcast-align -Wsign-promo -Wconversion" )
set( warnings "${warnings} -Winvalid-pch -Wdisabled-optimization -Wmissing-declarations" )
set( warnings "${warnings} -Woverloaded-virtual -Wnon-virtual-dtor -Wattributes" )
set( arch_flags "-msse3" )

if( OZ_CLANG )
  set( warnings "${warnings} -Wno-sign-conversion -Wno-invalid-offsetof" )
else()
  set( flags "${flags} -freciprocal-math" )
  set( warnings "${warnings} -Wlogical-op" )
  set( arch_flags "${arch_flags} -mfpmath=sse" )
  if( OZ_MINGW )
  else()
    set( flags "${flags} -fstrict-enums" )
  endif()
endif()

set( CMAKE_BUILD_TYPE Debug CACHE STRING "Debug RelWithDebInfo RelGenProf RelUseProf" )
set( CMAKE_CXX_FLAGS_DEBUG "-g3 -O1" CACHE STRING "" FORCE )
set( CMAKE_CXX_FLAGS_RELWITHDEBINFO "-g3 -O3" CACHE STRING "" FORCE )
set( CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG -O3" CACHE STRING "" FORCE )
# set( CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG -O3 -fdata-sections -ffunction-sections -fprofile-generate" CACHE STRING "" FORCE )
# set( CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG -O3 -fdata-sections -ffunction-sections -fprofile-use" CACHE STRING "" FORCE )

set( CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO "-Wl,--gc-sections" CACHE STRING "" FORCE )
set( CMAKE_EXE_LINKER_FLAGS_RELEASE "-Wl,--gc-sections" CACHE STRING "" FORCE )
# set( CMAKE_EXE_LINKER_FLAGS_RELEASE "-fprofile-generate -Wl,--gc-sections" CACHE STRING "" FORCE )
# set( CMAKE_EXE_LINKER_FLAGS_RELEASE "-fprofile-use -Wl,--gc-sections" CACHE STRING "" FORCE )

if( OZ_MINGW )
  set( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--enable-auto-import" )
else()
  set( CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fstack-protector-all" )
  set( CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -fstack-protector-all -flto" )
  set( CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -flto" )

  set( CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO "${CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO} -flto" )
  set( CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -flto" )
endif()

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

# installation
include( install_files.cmake )
