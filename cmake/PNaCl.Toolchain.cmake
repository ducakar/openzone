include(CMakeForceCompiler)

set(NACL                        ON)
set(PCH_DISABLE                 ON)

set(PLATFORM_EMBEDDED           ON)
set(PLATFORM_NAME               "PNaCl")
set(PLATFORM_TRIPLET            "pnacl")
set(PLATFORM_PREFIX             "$ENV{NACL_SDK_ROOT}/toolchain/linux_pnacl")
set(PLATFORM_PORTS_PREFIX       "${CMAKE_SOURCE_DIR}/ports/PNaCl")
set(PLATFORM_EXE_SUFFIX         ".pexe")

set(CMAKE_SYSTEM_NAME           "Linux" CACHE STRING "Target system.")
set(CMAKE_SYSTEM_PROCESSOR      "LLVM-IR" CACHE STRING "Target processor.")
set(CMAKE_FIND_ROOT_PATH        "${PLATFORM_PORTS_PREFIX}" "${PLATFORM_PREFIX}/x86_64_bc-nacl")
set(CMAKE_AR                    "${PLATFORM_PREFIX}/bin/${PLATFORM_TRIPLET}-ar" CACHE STRING "")
set(CMAKE_RANLIB                "${PLATFORM_PREFIX}/bin/${PLATFORM_TRIPLET}-ranlib" CACHE STRING "")
set(CMAKE_C_COMPILER            "${PLATFORM_PREFIX}/bin/${PLATFORM_TRIPLET}-clang")
set(CMAKE_CXX_COMPILER          "${PLATFORM_PREFIX}/bin/${PLATFORM_TRIPLET}-clang++")
set(CMAKE_C_FLAGS               "-U__STRICT_ANSI__" CACHE STRING "")
set(CMAKE_CXX_FLAGS             "-U__STRICT_ANSI__" CACHE STRING "")
set(CMAKE_C_FLAGS_RELEASE       "-O4 -ffast-math" CACHE STRING "")
set(CMAKE_CXX_FLAGS_RELEASE     "-O4 -ffast-math" CACHE STRING "")

cmake_force_c_compiler(         ${CMAKE_C_COMPILER} Clang)
cmake_force_cxx_compiler(       ${CMAKE_CXX_COMPILER} Clang)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

macro(pnacl_finalise _target)
  add_custom_command(TARGET ${_target} POST_BUILD
    COMMENT "Finalising ${_target}"
    COMMAND "${PLATFORM_PREFIX}/bin/${PLATFORM_TRIPLET}-finalize" "$<TARGET_FILE:${_target}>")
endmacro()

include_directories(SYSTEM $ENV{NACL_SDK_ROOT}/ports/include)
include_directories(SYSTEM $ENV{NACL_SDK_ROOT}/include)
include_directories(SYSTEM $ENV{NACL_SDK_ROOT}/include/newlib)
link_directories($ENV{NACL_SDK_ROOT}/ports/lib/newlib_pnacl/Release)
link_directories($ENV{NACL_SDK_ROOT}/lib/pnacl/Release)

set(ZLIB_INCLUDE_DIR      $ENV{NACL_SDK_ROOT}/ports/include CACHE STRING "")
set(ZLIB_LIBRARY          $ENV{NACL_SDK_ROOT}/ports/lib/newlib_pnacl/Release/libz.a CACHE STRING "")
set(PNG_PNG_INCLUDE_DIR   $ENV{NACL_SDK_ROOT}/ports/include)
set(PNG_LIBRARY           $ENV{NACL_SDK_ROOT}/ports/lib/newlib_pnacl/Release/libpng.a)
set(FREETYPE_INCLUDE_DIRS $ENV{NACL_SDK_ROOT}/ports/include)
set(FREETYPE_LIBRARY      $ENV{NACL_SDK_ROOT}/ports/lib/newlib_pnacl/Release/libfreetype.a)
set(LUA_INCLUDED IR       $ENV{NACL_SDK_ROOT}/ports/include)
set(LUA_LIBDIR            $ENV{NACL_SDK_ROOT}/ports/lib/newlib_pnacl/Release)
set(OPENAL_INCLUDE_DIR    $ENV{NACL_SDK_ROOT}/ports/include)
set(OPENAL_LIBRARY        $ENV{NACL_SDK_ROOT}/ports/lib/newlib_pnacl/Release/libopenal.a)
