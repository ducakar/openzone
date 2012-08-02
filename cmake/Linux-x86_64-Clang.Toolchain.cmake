set( CMAKE_SYSTEM_NAME          "Linux" CACHE STRING "Target system." )
set( CMAKE_SYSTEM_PROCESSOR     "x86_64" CACHE STRING "Target processor." )
set( CMAKE_C_COMPILER           "/usr/bin/clang" )
set( CMAKE_CXX_COMPILER         "/usr/bin/clang++" )

set( CMAKE_C_FLAGS              "-m64 -msse3" CACHE STRING "" )
set( CMAKE_CXX_FLAGS            "-m64 -msse3" CACHE STRING "" )
