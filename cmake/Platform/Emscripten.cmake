set( EMSCRIPTEN                                 1 )

set( CMAKE_EXECUTABLE_SUFFIX                    ".js" )
set( CMAKE_STATIC_LIBRARY_PREFIX                "lib" )
set( CMAKE_STATIC_LIBRARY_SUFFIX                ".js" )
set( CMAKE_FIND_LIBRARY_PREFIX                  "lib" )
set( CMAKE_FIND_LIBRARY_SUFFIX                  ".js" )

set( CMAKE_SYSTEM_PREFIX_PATH                   "/usr;/;/system" )
set( CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES   "/system/lib" )
set( CMAKE_C_IMPLICIT_INCLUDE_DIRECTORIES       "/system/include" )
set( CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES     "/system/include" )
