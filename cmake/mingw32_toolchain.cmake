# the name of the target operating system
set( CMAKE_SYSTEM_NAME Windows )

# which compilers to use for C and C++
set( CMAKE_C_COMPILER i486-mingw32-gcc )
set( CMAKE_CXX_COMPILER i486-mingw32-g++ )
set( CMAKE_RC_COMPILER i486-mingw32-windres )

# here is the target environment located
set( CMAKE_FIND_ROOT_PATH /usr/i486-mingw32 )

# adjust the default behaviour of the FIND_XXX() commands:
# search headers and libraries in the target environment, search
# programs in the host environment
set( CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER )
set( CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY )
set( CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY )
