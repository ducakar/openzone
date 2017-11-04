cmake_minimum_required(VERSION 3.0)

project(openzone)

#
# Configuration options.
#

set(OZ_VERSION 0.3.95)

option(OZ_ALLOCATOR "Overload new/delete with diagnostics and allocations tracking." OFF)
option(OZ_SIMD "Use SIMD implementation of linear algebra classes." OFF)
option(OZ_GL_ES "Use OpenGL ES 2.0 instead of OpenGL 2.1." OFF)
option(OZ_LUAJIT "Use use LuaJIT instead of official Lua." OFF)
option(OZ_TOOLS "Build engine tools required for game data creation." OFF)
option(OZ_TESTS "Build test programs (that should only be used by me)." OFF)

#
# Internal configuration.
#

option(OZ_BUNDLE "Adjust installation for OpenZone multi-platform bundle ZIP." OFF)
mark_as_advanced(OZ_BUNDLE)

set(OZ_PLATFORM_NAME ${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR})

# Emscripten uses stock toolchain file, we have to set these things manually.
if(EMSCRIPTEN)
  set(PLATFORM_NAME         "Emscripten")
  set(PLATFORM_EMBEDDED     ON)
  set(PLATFORM_PORTS_PREFIX ${CMAKE_SOURCE_DIR}/ports/Emscripten/usr)

  set(CMAKE_FIND_ROOT_PATH  ${CMAKE_FIND_ROOT_PATH};${PLATFORM_PORTS_PREFIX})
  set(CMAKE_INCLUDE_PATH    ${PLATFORM_PORTS_PREFIX}/include)
  set(CMAKE_LIBRARY_PATH    ${PLATFORM_PORTS_PREFIX}/lib)
  set(CMAKE_CXX_FLAGS       "${CMAKE_CXX_FLAGS} -s USE_PTHREADS=1 -Wno-disabled-macro-expansion")

  set(ZLIB_ROOT             ${PLATFORM_PORTS_PREFIX})
  set(LUA_MATH_LIBRARY      "-lm")

  # For QtCreator to find headers.
  include_directories(SYSTEM /usr/lib/emscripten/system/include/libcxx)
  include_directories(SYSTEM /usr/lib/emscripten/system/include/compat)
  add_definitions(-D__EMSCRIPTEN__)
endif()

if(PLATFORM_EMBEDDED)
  set(OZ_PLATFORM_NAME ${PLATFORM_NAME}-${CMAKE_SYSTEM_PROCESSOR})
  set(OZ_GL_ES ON)
  set(OZ_TOOLS OFF)
endif()

if(OZ_BUNDLE)
  set(OZ_PREFIX "")
  set(OZ_BINARY_SUBDIR "/${OZ_PLATFORM_NAME}")
  set(CMAKE_INSTALL_RPATH "$ORIGIN")
else()
  set(OZ_PREFIX "${CMAKE_INSTALL_PREFIX}")
  set(OZ_BINARY_SUBDIR "")
endif()

include(cmake/PCH.cmake)
include(FindPkgConfig)

#
# Libraries.
#

# Set-up include and library directories for different toolchains.
if(EMSCRIPTEN)
  set(ENV{PKG_CONFIG_PATH} ${PLATFORM_PORTS_PREFIX}/lib/pkgconfig)
elseif(ANDROID)
elseif(NACL)
  set(ENV{PKG_CONFIG_PATH} ${CMAKE_FIND_ROOT_PATH}/usr/lib/pkgconfig)
elseif(WIN32)
  set(ENV{PKG_CONFIG_PATH} ${CMAKE_FIND_ROOT_PATH}/lib/pkgconfig)
else()
  pkg_check_modules(PULSE_SIMPLE REQUIRED libpulse-simple)
endif()

find_package(ZLIB REQUIRED)
pkg_check_modules(PHYSFS REQUIRED physfs)

if(OZ_LUAJIT)
  pkg_check_modules(LUAJIT REQUIRED luajit)
  set(LUA_LIBRARIES ${LUAJIT_LIBRARIES})
  include_directories(SYSTEM ${LUAJIT_INCLUDE_DIRS})
elseif(EMSCRIPTEN)
  find_package(Lua REQUIRED)
  include_directories(SYSTEM ${LUA_INCLUDE_DIR})
else()
  pkg_check_modules(LUA REQUIRED lua)
  include_directories(SYSTEM ${LUA_INCLUDE_DIRS})
endif()

if(EMSCRIPTEN)
  pkg_check_modules(SDL REQUIRED sdl)
  set(SDL_TTF_LIBRARIES "")
else()
  pkg_check_modules(SDL REQUIRED sdl2)
  pkg_check_modules(SDL_TTF REQUIRED SDL2_ttf)
  list(REMOVE_ITEM SDL_LIBRARIES ppapi_simple)
  list(REMOVE_ITEM SDL_TTF_LIBRARIES ppapi_simple)
endif()

if(OZ_GL_ES)
  # On embedded platforms, GLES is part of system libraries and is guaranteed to exist in toolchain
  # or platform directory, no need to explicitly check existence or to detect library names.
  if(ANDROID)
    set(GL_LIBRARIES GLESv2)
  elseif(EMSCRIPTEN)
    set(GL_LIBRARIES "")
  elseif(NACL)
    set(GL_LIBRARIES ppapi_gles2)
  else()
    pkg_check_modules(GL REQUIRED glesv2)
  endif()
else()
  find_package(OpenGL REQUIRED)
  set(GL_LIBRARIES ${OPENGL_gl_LIBRARY})
endif()

find_package(OpenAL REQUIRED)
find_package(PNG REQUIRED)
pkg_check_modules(OGG REQUIRED ogg)
pkg_check_modules(OPUS REQUIRED opus)
pkg_check_modules(OPUSFILE REQUIRED opusfile)
pkg_check_modules(VORBISFILE REQUIRED vorbisfile)

include_directories(SYSTEM ${OPUSFILE_INCLUDE_DIRS})

# On embedded platforms, linking is mostly static, so indirect dependencies must be linked too.
if(PLATFORM_EMBEDDED)
  # Emscripten has SDL_ttf built-in and doesn't need Freetype.
  if(NOT EMSCRIPTEN)
    find_package(BZip2 REQUIRED)    # Required by FreeType.
    find_package(Freetype REQUIRED) # Required by SDL_ttf.

    list(APPEND SDL_TTF_LIBRARIES ${FREETYPE_LIBRARIES} ${BZIP2_LIBRARIES} ${PNG_LIBRARIES})
  endif()
endif()

if(OZ_TOOLS)
  pkg_check_modules(ASSIMP REQUIRED assimp)

  find_library(FREEIMAGE_LIBRARY NAMES freeimage FreeImage)
  find_library(NOISE_LIBRARY noise)
  find_library(SQUISH_LIBRARY squish)

  if(NOT FREEIMAGE_LIBRARY)
    message(FATAL_ERROR "FreeImage library is missing")
  endif()
  if(NOT NOISE_LIBRARY)
    message(FATAL_ERROR "libnoise library is missing")
  endif()
  if(NOT SQUISH_LIBRARY)
    message(FATAL_ERROR "libsquish library is missing")
  endif()

  mark_as_advanced(FREEIMAGE_LIBRARY NOISE_LIBRARY SQUISH_LIBRARY)
endif()

mark_as_advanced(MINGW32_LIBRARY)

#
# Compiler flags.
#

# Local libmad, faad and eSpeak headers.
include_directories(SYSTEM etc/include)

set(flags "-std=c++17 -fvisibility-inlines-hidden -fsized-deallocation -fstrict-enums -ffast-math")

if(BUILD_SHARED_LIBS)
  set(flags "${flags} -fPIC")
endif()

# Static analyser.
#set(flags "${flags} -Qunused-arguments --analyze")
# Address sanitizer.
#set(flags "${flags} -fsanitize=address")
# Thread sanitizer.
#set(flags "${flags} -fsanitize=thread -fpie -fPIC")
# Undefined behaviour sanitizer.
#set(flags "${flags} -fsanitize=undefined")

# Additional flags.
if(${CMAKE_CXX_COMPILER_ID} STREQUAL Clang)
  set(flags "${flags} -Weverything -Wno-c++98-compat -Wno-c++98-compat-pedantic")
  set(flags "${flags} -Wno-exit-time-destructors -Wno-global-constructors")
  set(flags "${flags} -Wno-date-time -Wno-padded -Wno-switch-enum -Wno-sign-conversion")
  set(flags "${flags} -Wno-float-equal -Wno-double-promotion -Wno-zero-as-null-pointer-constant")
else()
  set(flags "${flags} -Wall -Wextra -Wcast-align -Winit-self -Wlogical-op -Wundef -Winvalid-pch")
  set(flags "${flags} -Wmissing-declarations -Wnon-virtual-dtor -Woverloaded-virtual -Wformat=2")
  set(flags "${flags} -Wno-format-zero-length -Wmissing-format-attribute -Wduplicated-cond")
endif()

set(CMAKE_CXX_FLAGS "${flags} ${CMAKE_CXX_FLAGS}")

# Strings for BuildInfo class.
string(TOUPPER "CMAKE_CXX_FLAGS_${CMAKE_BUILD_TYPE}" OZ_CXX_FLAGS)
string(STRIP "${CMAKE_CXX_FLAGS} ${${OZ_CXX_FLAGS}}" OZ_CXX_FLAGS)

get_directory_property(_includes INCLUDE_DIRECTORIES)
foreach(_include ${_includes})
  set(OZ_CXX_FLAGS "${OZ_CXX_FLAGS} -I${_include}")
endforeach()

get_directory_property(_defines COMPILE_DEFINITIONS)
foreach(_define ${_defines})
  set(OZ_CXX_FLAGS "${OZ_CXX_FLAGS} -D${_define}")
endforeach()

site_name(OZ_HOSTNAME)
mark_as_advanced(OZ_HOSTNAME)

#
# Sources.
#

add_subdirectory(src)

# This is to show these files in Qt Creator.
file(GLOB docFiles AUTHORS COPYING *.md)
file(GLOB_RECURSE dataFiles data/*.txt data/*.json data/*.lua data/*.vert data/*.frag)
add_custom_target(dataFiles SOURCES ${docFiles} ${dataFiles})

#
# Data files.
#

# Application launcher and icon.
if(NOT OZ_BUNDLE)
  install(DIRECTORY ${CMAKE_SOURCE_DIR}/share/applications ${CMAKE_SOURCE_DIR}/share/pixmaps
          DESTINATION share)
endif()

# Documentation, game data and libraries for a standalone bundle.
if(OZ_BUNDLE)
  file(GLOB readmes doc/*.html)
  file(GLOB dataFiles share/openzone/*.zip)

  install(FILES AUTHORS COPYING README.md ChangeLog.md TODO.md DESTINATION doc)
  install(FILES ${dataFiles} DESTINATION share/openzone)
  install(FILES ${readmes} DESTINATION .)
  install(DIRECTORY doc/licences DESTINATION .)

  install(DIRECTORY lib/${OZ_PLATFORM_NAME} DESTINATION bin USE_SOURCE_PERMISSIONS)
endif()

#
# Android project.
#

if(ANDROID)
  file(GLOB dataFiles share/openzone/*.7z)

  add_custom_command(OUTPUT ../Android
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/etc/android-project ../Android
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/share/openzone
            ../Android/assets/share/openzone)
  add_custom_target(android_project ALL DEPENDS ${CMAKE_BINARY_DIR}/../Android)
endif()
