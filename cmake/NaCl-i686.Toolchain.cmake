set( NACL ON )

set( PLATFORM_EMBEDDED          ON )
set( PLATFORM_TRIPLET           "i686-nacl" )
set( PLATFORM_PREFIX            "/home/davorin/Projects/nacl_sdk/pepper_23/toolchain/linux_x86_newlib" )
set( PLATFORM_PORTS_PREFIX      "${CMAKE_SOURCE_DIR}/ports/NaCl-i686" )

set( CMAKE_SYSTEM_NAME          "Linux" CACHE STRING "Target system." )
set( CMAKE_SYSTEM_PROCESSOR     "i686" CACHE STRING "Target processor." )
set( CMAKE_FIND_ROOT_PATH       "${PLATFORM_PORTS_PREFIX};${PLATFORM_PREFIX}/x86_64-nacl" )
set( CMAKE_C_COMPILER           "${PLATFORM_PREFIX}/bin/${PLATFORM_TRIPLET}-gcc" )
set( CMAKE_CXX_COMPILER         "${PLATFORM_PREFIX}/bin/${PLATFORM_TRIPLET}-g++" )
set( CMAKE_C_FLAGS              "-msse3 -mfpmath=sse" CACHE STRING "" )
set( CMAKE_CXX_FLAGS            "-msse3 -mfpmath=sse" CACHE STRING "" )
