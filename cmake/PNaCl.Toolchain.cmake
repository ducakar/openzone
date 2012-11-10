set( NACL ON )

set( PLATFORM_TRIPLET           "pnacl" )
set( PLATFORM_PREFIX            "/home/davorin/Projects/nacl_sdk/pepper_canary/toolchain/linux_x86_pnacl/newlib" )
set( PLATFORM_PORTS_PREFIX      "${CMAKE_SOURCE_DIR}/ports/PNaCl" )

set( CMAKE_SYSTEM_NAME          "Linux" CACHE STRING "Target system." )
set( CMAKE_SYSTEM_PROCESSOR     "LLVM" CACHE STRING "Target processor." )
set( CMAKE_FIND_ROOT_PATH       "${PLATFORM_PORTS_PREFIX};${PLATFORM_PREFIX}/usr" )
set( CMAKE_FIND_ROOT_PATH       "${CMAKE_FIND_ROOT_PATH};${PLATFORM_PREFIX}/sdk" )
set( CMAKE_C_COMPILER           "${PLATFORM_PREFIX}/bin/${PLATFORM_TRIPLET}-clang" )
set( CMAKE_CXX_COMPILER         "${PLATFORM_PREFIX}/bin/${PLATFORM_TRIPLET}-clang++" )
set( CMAKE_C_FLAGS              "" CACHE STRING "" )
set( CMAKE_CXX_FLAGS            "" CACHE STRING "" )
