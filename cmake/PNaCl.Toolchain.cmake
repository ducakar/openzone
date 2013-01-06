set( NACL                       ON )

set( PLATFORM_NAME              "PNaCl" )
set( PLATFORM_EMBEDDED          ON )
set( PLATFORM_TRIPLET           "pnacl" )
set( PLATFORM_PREFIX            "$ENV{NACL_SDK_ROOT}/toolchain/linux_x86_pnacl/newlib" )
set( PLATFORM_PORTS_PREFIX      "${CMAKE_SOURCE_DIR}/ports/PNaCl" )
set( PLATFORM_EXE_SUFFIX        ".pexe" )

set( CMAKE_SYSTEM_NAME          "Linux" CACHE STRING "Target system." )
set( CMAKE_SYSTEM_PROCESSOR     "LLVM-IR" CACHE STRING "Target processor." )
set( CMAKE_FIND_ROOT_PATH       "${PLATFORM_PORTS_PREFIX};${PLATFORM_PREFIX}/usr" )
set( CMAKE_FIND_ROOT_PATH       "${CMAKE_FIND_ROOT_PATH};${PLATFORM_PREFIX}/sdk" )
set( CMAKE_C_COMPILER           "${PLATFORM_PREFIX}/bin/${PLATFORM_TRIPLET}-clang" )
set( CMAKE_CXX_COMPILER         "${PLATFORM_PREFIX}/bin/${PLATFORM_TRIPLET}-clang++" )
set( CMAKE_AR                   "${PLATFORM_PREFIX}/bin/${PLATFORM_TRIPLET}-ar" CACHE STRING "")
set( CMAKE_RANLIB               "${PLATFORM_PREFIX}/bin/${PLATFORM_TRIPLET}-ranlib" CACHE STRING "")

set( PCH_DISABLE                ON )

include( CMakeForceCompiler )
cmake_force_c_compiler( ${CMAKE_C_COMPILER} Clang )
cmake_force_cxx_compiler( ${CMAKE_CXX_COMPILER} Clang )

set( CMAKE_C_COMPILER_VERSION   3.2 )
set( CMAKE_CXX_COMPILER_VERSION 3.2 )
