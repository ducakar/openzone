set( PLATFORM_TRIPLET           "i686-w64-mingw32" )
set( PLATFORM_PREFIX            "/usr/${PLATFORM_TRIPLET}" )

set( CMAKE_SYSTEM_NAME          "Windows" CACHE STRING "Target system." )
set( CMAKE_SYSTEM_PROCESSOR     "i686" CACHE STRING "Target processor." )
set( CMAKE_FIND_ROOT_PATH       "${PLATFORM_PREFIX}" )
set( CMAKE_AR                   "/usr/bin/${PLATFORM_TRIPLET}-gcc-ar" CACHE STRING "" )
set( CMAKE_NM                   "/usr/bin/${PLATFORM_TRIPLET}-gcc-nm" CACHE STRING "" )
set( CMAKE_RANLIB               "/usr/bin/${PLATFORM_TRIPLET}-gcc-ranlib" CACHE STRING "" )
set( CMAKE_RC_COMPILER          "/usr/bin/${PLATFORM_TRIPLET}-windres" CACHE STRING "" )
set( CMAKE_STRIP                "/usr/bin/${PLATFORM_TRIPLET}-strip" CACHE STRING "" )
set( CMAKE_C_COMPILER           "/usr/bin/${PLATFORM_TRIPLET}-gcc" )
set( CMAKE_CXX_COMPILER         "/usr/bin/${PLATFORM_TRIPLET}-g++" )
set( CMAKE_C_FLAGS              "-march=i686 -m32 -msse3 -mfpmath=sse" CACHE STRING "" )
set( CMAKE_CXX_FLAGS            "-march=i686 -m32 -msse3 -mfpmath=sse" CACHE STRING "" )
# FIXME Enable "-flto" when fixed.
# FIXME "-Ofast" produces SIGGEGVs all around valid code.
set( CMAKE_C_FLAGS_RELEASE      "-O" CACHE STRING "" )
set( CMAKE_CXX_FLAGS_RELEASE    "-O" CACHE STRING "" )

set( CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER )
set( CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY )
set( CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY )
set( CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY )

# CMake fails to detect this.
set( OPENAL_INCLUDE_DIR         "${CMAKE_FIND_ROOT_PATH}/include/AL" )
