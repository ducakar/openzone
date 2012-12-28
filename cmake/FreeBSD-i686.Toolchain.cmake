set( PLATFORM_NAME              "FreeBSD" )
set( PLATFORM_PREFIX            "" )

set( CMAKE_SYSTEM_NAME          "Linux" CACHE STRING "Target system." )
set( CMAKE_SYSTEM_PROCESSOR     "i686" CACHE STRING "Target processor." )
set( CMAKE_C_COMPILER           "/usr/local/bin/clang" )
set( CMAKE_CXX_COMPILER         "/usr/local/bin/clang++" )
set( CMAKE_C_FLAGS              "-march=i686 -m32 -msse3" CACHE STRING "" )
set( CMAKE_CXX_FLAGS            "-march=i686 -m32 -msse3" CACHE STRING "" )
