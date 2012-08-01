set( CMAKE_SYSTEM_NAME          "Linux" )
set( CMAKE_SYSTEM_PROCESSOR     "i686" )
set( CMAKE_C_COMPILER           "/usr/bin/clang" )
set( CMAKE_CXX_COMPILER         "/usr/bin/clang++" )

set( CMAKE_C_FLAGS              "-m32 -march=i686 -mtune=generic -msse3" CACHE STRING "" )
set( CMAKE_CXX_FLAGS            "-m32 -march=i686 -mtune=generic -msse3" CACHE STRING "" )
