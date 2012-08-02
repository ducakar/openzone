set( CMAKE_SYSTEM_NAME          "Linux" CACHE STRING "Target system." )
set( CMAKE_SYSTEM_PROCESSOR     "i686" CACHE STRING "Target processor." )
set( CMAKE_C_COMPILER           "/usr/bin/clang" )
set( CMAKE_CXX_COMPILER         "/usr/bin/clang++" )

set( CMAKE_C_FLAGS              "-m32 -march=i686 -mtune=generic -msse3" CACHE STRING "" )
set( CMAKE_CXX_FLAGS            "-m32 -march=i686 -mtune=generic -msse3" CACHE STRING "" )
