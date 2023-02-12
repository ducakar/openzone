cmake_minimum_required(VERSION 3.18)

project(openzone VERSION "0.3.95")

#
# Configuration options.
#

option(OZ_SIMD "Use SIMD implementation of linear algebra classes." OFF)
option(OZ_GL_ES "Use OpenGL ES 2.0 instead of OpenGL 2.1." OFF)
option(OZ_LUAJIT "Use use LuaJIT instead of official Lua." OFF)
option(OZ_TOOLS "Build engine tools required for game data creation." OFF)
option(OZ_BUNDLE "Adjust installation for OpenZone multi-platform bundle ZIP." OFF)

# get_cmake_property(_variableNames VARIABLES)
# list (SORT _variableNames)
# foreach (_variableName ${_variableNames})
#     message(STATUS "${_variableName}=${${_variableName}}")
# endforeach()

#
# Internal configuration.
#

set(OZ_PLATFORM_NAME ${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR})

# Emscripten uses stock toolchain file, we have to set these things manually.
if(EMSCRIPTEN)
  set(CMAKE_EXECUTABLE_SUFFIX ".html")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -sPROXY_TO_PTHREAD=1 -sPTHREAD_POOL_SIZE=8")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -sUSE_ZLIB=1 -sUSE_SDL=2 -sUSE_SDL_TTF=2 -sUSE_LIBPNG=1")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -sUSE_OGG=1 -sUSE_VORBIS=1")
  add_compile_options(-Wno-unused-command-line-argument -Wno-disabled-macro-expansion)
  add_compile_options(-Wno-unsafe-buffer-usage)
  # For clangd.
  add_definitions(-D__EMSCRIPTEN__)
endif()

if(OZ_BUNDLE)
  set(CMAKE_INSTALL_PREFIX "")
  set(CMAKE_INSTALL_RPATH "$ORIGIN")
  set(OZ_BINARY_SUBDIR "/${OZ_PLATFORM_NAME}")
else()
  set(OZ_BINARY_SUBDIR "")
endif()

include(FindPkgConfig)

#
# Libraries.
#

# Silence a warning in SDL2_TTF CMake config file.
if (NOT DEFINED CMAKE_MODULE_PATH)
  set(CMAKE_MODULE_PATH "")
endif()

# Set-up include and library directories for different toolchains.
if(EMSCRIPTEN)
  if(DEFINED VCPKG_INSTALLED_DIR)
    link_directories("${VCPKG_INSTALLED_DIR}/wasm32-emscripten/lib")
  endif()
elseif(WIN32)
  if(DEFINED VCPKG_INSTALLED_DIR)
    link_directories("${VCPKG_INSTALLED_DIR}/x64-windows/lib")
    link_libraries(-lssp)
  else()
    set(CMAKE_FIND_ROOT_PATH /usr/x86_64-w64-mingw32)
    set(ENV{PKG_CONFIG_PATH} ${CMAKE_FIND_ROOT_PATH}/lib/pkgconfig)
  endif()
else()
  find_package(Threads REQUIRED)
  find_package(ALSA REQUIRED)
endif()

find_package(PhysFS REQUIRED)

if(EMSCRIPTEN)
  find_library(OPUSFILE_LIBRARIES opusfile REQUIRED)
else()
  find_package(OpenAL CONFIG REQUIRED)
  find_package(ZLIB REQUIRED)
  find_package(SDL2 REQUIRED)
  find_package(SDL2_ttf REQUIRED)
  find_package(PNG REQUIRED)
  find_package(Ogg REQUIRED)
  pkg_check_modules(OPUSFILE REQUIRED opusfile)
  pkg_check_modules(VORBISFILE REQUIRED vorbisfile)
endif()

pkg_check_modules(OPUS REQUIRED opus)
include_directories(SYSTEM ${OPUS_INCLUDE_DIRS})

if(OZ_LUAJIT)
  pkg_check_modules(LUAJIT REQUIRED luajit)
  set(LUA_LIBRARIES ${LUAJIT_LIBRARIES})
  include_directories(SYSTEM ${LUAJIT_INCLUDE_DIRS})
else()
  find_package(Lua REQUIRED)
  include_directories(SYSTEM ${LUA_INCLUDE_DIR})
endif()

if(NOT EMSCRIPTEN)
  if(OZ_GL_ES)
    # On embedded platforms, GLES is part of system libraries and is guaranteed to exist in toolchain
    # or platform directory, no need to explicitly check existence or to detect library names.
    pkg_check_modules(GL REQUIRED glesv2)
  else()
    find_package(OpenGL REQUIRED)
    set(GL_LIBRARIES OpenGL::GL)
  endif()
endif()

if(OZ_TOOLS)
  find_package(assimp REQUIRED)
  find_library(FREEIMAGE_LIBRARY NAMES freeimage FreeImage REQUIRED)
  find_library(NOISE_LIBRARY noise REQUIRED)
  find_library(SQUISH_LIBRARY squish REQUIRED)
endif()

#
# Compiler flags.
#

# External library headers.
if(UNIX)
  include_directories(SYSTEM ext/gamemode/lib)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-fPIC -fsized-deallocation -fstrict-enums -fno-rtti -fno-exceptions -ffast-math)

# Static analyser.
# add_compile_options(-Qunused-arguments --analyze)
# Address sanitizer and undefined behaviour sanitizers.
# add_compile_options(-fsanitize=address,leak,undefined)
# add_link_options(-fsanitize=address,leak,undefined)
# Thread sanitizer.
# add_compile_options(-fsanitize=thread,undefined)
# add_link_options(-fsanitize=thread,undefined)

# Warnings.
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  add_compile_options(-Weverything -Wno-c++98-compat -Wno-c++98-compat-pedantic)
  add_compile_options(-Wno-exit-time-destructors -Wno-global-constructors)
  add_compile_options(-Wno-date-time -Wno-padded -Wno-switch-enum -Wno-sign-conversion)
  add_compile_options(-Wno-float-equal -Wno-double-promotion -Wno-zero-as-null-pointer-constant)
else()
  add_compile_options(-Wall -Wextra -Wcast-align -Winit-self -Wlogical-op -Wundef -Winvalid-pch)
  add_compile_options(-Wmissing-declarations -Wnon-virtual-dtor -Woverloaded-virtual -Wformat=2)
  add_compile_options(-Wno-format-zero-length -Wmissing-format-attribute -Wduplicated-cond)
endif()

# Strings for BuildInfo class.
get_property(compileOptions DIRECTORY PROPERTY COMPILE_OPTIONS)
list(JOIN compileOptions " " compileOptions)
string(TOUPPER "CMAKE_CXX_FLAGS_${CMAKE_BUILD_TYPE}" OZ_COMPILE_OPTIONS)
string(STRIP "${compileOptions} ${CMAKE_CXX_FLAGS} ${${OZ_COMPILE_OPTIONS}}" OZ_COMPILE_OPTIONS)

site_name(OZ_HOSTNAME)
mark_as_advanced(OZ_HOSTNAME)

# Enable unittests.
enable_testing()

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
  install(DIRECTORY ${CMAKE_SOURCE_DIR}/share/applications DESTINATION share)
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
