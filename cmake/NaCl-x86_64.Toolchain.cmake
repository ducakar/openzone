set( NACL ON )

set( PLATFORM_TRIPLET           "x86_64-nacl" )
set( PLATFORM_PREFIX            "/home/davorin/Projects/nacl_sdk/pepper_22/toolchain/linux_x86_newlib" )

set( CMAKE_SYSTEM_NAME          "Linux" CACHE STRING "Target system." )
set( CMAKE_SYSTEM_PROCESSOR     "x86_64" CACHE STRING "Target processor." )
set( CMAKE_FIND_ROOT_PATH       "${PLATFORM_PREFIX}/${PLATFORM_TRIPLET}" )
set( CMAKE_C_COMPILER           "${PLATFORM_PREFIX}/bin/${PLATFORM_TRIPLET}-gcc" )
set( CMAKE_CXX_COMPILER         "${PLATFORM_PREFIX}/bin/${PLATFORM_TRIPLET}-g++" )

set( CMAKE_C_FLAGS              "-march=x86-64 -msse3" CACHE STRING "" )
set( CMAKE_CXX_FLAGS            "-march=x86-64 -msse3" CACHE STRING "" )
