set( NACL ON )
set( NACL64 ON )

set( PLATFORM_TRIPLET           "x86_64-nacl" )
set( PLATFORM_PREFIX            "/home/davorin/Projects/nacl_sdk/pepper_19/toolchain/linux_x86_newlib" )

set( CMAKE_SYSTEM_NAME          Linux )
set( CMAKE_FIND_ROOT_PATH       "${PLATFORM_PREFIX}/${PLATFORM_TRIPLET}" )
set( CMAKE_C_COMPILER           "${PLATFORM_PREFIX}/bin/${PLATFORM_TRIPLET}-gcc" )
set( CMAKE_CXX_COMPILER         "${PLATFORM_PREFIX}/bin/${PLATFORM_TRIPLET}-g++" )
set( CMAKE_STRIP                "${PLATFORM_PREFIX}/bin/${PLATFORM_TRIPLET}-strip" )
