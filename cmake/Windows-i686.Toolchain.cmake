set( MINGW32 ON )

set( PLATFORM_TRIPLET           "i486-mingw32" )
set( PLATFORM_PREFIX            "/usr/${PLATFORM_TRIPLET}" )
set( PLATFORM_TOOL_PREFIX       "${PLATFORM_PREFIX}" )

set( CMAKE_SYSTEM_NAME          "Windows" CACHE STRING "Target system." )
set( CMAKE_SYSTEM_PROCESSOR     "i686" CACHE STRING "Target processor." )
set( CMAKE_FIND_ROOT_PATH       "${PLATFORM_PREFIX}" )
set( CMAKE_RC_COMPILER          "${PLATFORM_TRIPLET}-windres" )
set( CMAKE_STRIP                "${PLATFORM_TRIPLET}-strip" )
set( CMAKE_C_COMPILER           "${PLATFORM_TRIPLET}-gcc" )
set( CMAKE_CXX_COMPILER         "${PLATFORM_TRIPLET}-g++" )
set( CMAKE_C_FLAGS              "-march=i686 -m32 -msse3 -mfpmath=sse" CACHE STRING "" )
set( CMAKE_CXX_FLAGS            "-march=i686 -m32 -msse3 -mfpmath=sse" CACHE STRING "" )

# CMake doesn't detect those correctly.
set( SDL_INCLUDE_DIR            "${CMAKE_FIND_ROOT_PATH}/include/SDL" CACHE PATH "" )
set( SDLTTF_INCLUDE_DIR         "${CMAKE_FIND_ROOT_PATH}/include/SDL" CACHE PATH "" )
set( OPENAL_INCLUDE_DIR         "${CMAKE_FIND_ROOT_PATH}/include" CACHE PATH "" )
set( FREEIMAGE_LIBRARY          "${CMAKE_FIND_ROOT_PATH}/lib/FreeImage.lib" CACHE PATH "" )
