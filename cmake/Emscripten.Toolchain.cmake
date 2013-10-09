set( EMSCRIPTEN                               ON )
set( PCH_DISABLE                              ON )

set( PLATFORM_EMBEDDED                        ON )
set( PLATFORM_NAME                            "Emscripten" )
set( PLATFORM_TRIPLET                         "emscripten" )
set( PLATFORM_PREFIX                          "$ENV{EMSCRIPTEN}" )
set( PLATFORM_PORTS_PREFIX                    "${CMAKE_SOURCE_DIR}/ports/Emscripten" )

set( CMAKE_SYSTEM_NAME                        "Emscripten" CACHE STRING "Target system." )
set( CMAKE_SYSTEM_PROCESSOR                   "JavaScript" CACHE STRING "Target processor." )
set( CMAKE_FIND_ROOT_PATH                     "${PLATFORM_PORTS_PREFIX};${PLATFORM_PREFIX}/system" CACHE STRING "" )

set( CMAKE_AR                                 "$ENV{EMSCRIPTEN}/emar")
set( CMAKE_RANLIB                             "$ENV{EMSCRIPTEN}/emranlib")
set( CMAKE_C_COMPILER                         "${PLATFORM_PREFIX}/emcc" )
set( CMAKE_CXX_COMPILER                       "${PLATFORM_PREFIX}/em++" )
set( CMAKE_C_ARCHIVE_CREATE                     "${CMAKE_C_COMPILER} -o <TARGET> -emit-llvm <LINK_FLAGS> <OBJECTS>" )
set( CMAKE_CXX_ARCHIVE_CREATE                   "${CMAKE_CXX_COMPILER} -o <TARGET> -emit-llvm <LINK_FLAGS> <OBJECTS>" )

set( CMAKE_C_FLAGS                            "-Qunused-arguments -U__STRICT_ANSI__ -Wno-warn-absolute-paths" CACHE STRING "" )
set( CMAKE_CXX_FLAGS                          "-Qunused-arguments -U__STRICT_ANSI__ -Wno-warn-absolute-paths" CACHE STRING "" )
set( CMAKE_C_FLAGS_RELEASE                    "-DNDEBUG" CACHE STRING "" )
set( CMAKE_C_FLAGS_MINSIZEREL                 "-DNDEBUG" CACHE STRING "" )
set( CMAKE_C_FLAGS_RELWITHDEBINFO             "" CACHE STRING "" )
set( CMAKE_CXX_FLAGS_RELEASE                  "-DNDEBUG" CACHE STRING "" )
set( CMAKE_CXX_FLAGS_MINSIZEREL               "-DNDEBUG" CACHE STRING "" )
set( CMAKE_CXX_FLAGS_RELWITHDEBINFO           "" CACHE STRING "" )

set( CMAKE_EXE_LINKER_FLAGS_RELEASE           "-O2" CACHE STRING "" )
set( CMAKE_EXE_LINKER_FLAGS_MINSIZEREL        "-O2" CACHE STRING "" )
set( CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO    "-O2" CACHE STRING "" )
set( CMAKE_SHARED_LINKER_FLAGS_RELEASE        "-O2" CACHE STRING "" )
set( CMAKE_SHARED_LINKER_FLAGS_MINSIZEREL     "-O2" CACHE STRING "" )
set( CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO "-O2" CACHE STRING "" )
set( CMAKE_MODULE_LINKER_FLAGS_RELEASE        "-O2" CACHE STRING "" )
set( CMAKE_MODULE_LINKER_FLAGS_MINSIZEREL     "-O2" CACHE STRING "" )
set( CMAKE_MODULE_LINKER_FLAGS_RELWITHDEBINFO "-O2" CACHE STRING "" )

set( CMAKE_FIND_ROOT_PATH_MODE_PROGRAM        NEVER )
set( CMAKE_FIND_ROOT_PATH_MODE_LIBRARY        ONLY )
set( CMAKE_FIND_ROOT_PATH_MODE_INCLUDE        ONLY )
set( CMAKE_FIND_ROOT_PATH_MODE_PACKAGE        ONLY )

include( CMakeForceCompiler )
cmake_force_c_compiler( ${CMAKE_C_COMPILER} Clang )
cmake_force_cxx_compiler( ${CMAKE_CXX_COMPILER} Clang )

# CMake fails to detect this.
set( FREETYPE_INCLUDE_DIRS                    "${PLATFORM_PORTS_PREFIX}/usr/include" )

# Dummy for Emscripten.
set( SDL_INCLUDE_DIRS                         "${PLATFORM_PREFIX}/system/include/SDL" )
set( SDL_LIBRARIES                            "" )
