set( ANDROID                    ON )

set( PLATFORM_EMBEDDED          ON )
set( PLATFORM_TRIPLET           "i686-linux-android" )
set( PLATFORM_PROC_PREFIX       "x86" )
set( PLATFORM_PREFIX            "/opt/android-ndk/platforms/android-14/arch-x86" )
set( PLATFORM_TOOL_PREFIX       "/opt/android-ndk/toolchains/x86-4.6/prebuilt/linux-x86" )
set( PLATFORM_STL_PREFIX        "/opt/android-ndk/sources/cxx-stl/gnu-libstdc++/4.6" )
set( PLATFORM_PORTS_PREFIX      "${CMAKE_SOURCE_DIR}/ports/Android14-i686" )
set( PLATFORM_FLAGS             "-fPIC --sysroot=${PLATFORM_PREFIX}" )
set( PLATFORM_FLAGS             "${PLATFORM_FLAGS} -isystem ${PLATFORM_STL_PREFIX}/include" )
set( PLATFORM_FLAGS             "${PLATFORM_FLAGS} -isystem ${PLATFORM_STL_PREFIX}/libs/x86/include" )
set( PLATFORM_STL_LIBRARY       "${PLATFORM_STL_PREFIX}/libs/x86/libgnustl_shared.so" )

set( CMAKE_SYSTEM_NAME          "Linux" CACHE STRING "Target system." )
set( CMAKE_SYSTEM_PROCESSOR     "i686" CACHE STRING "Target processor." )
set( CMAKE_FIND_ROOT_PATH       "${PLATFORM_PORTS_PREFIX};${PLATFORM_PREFIX}" )
set( CMAKE_C_COMPILER           "${PLATFORM_TOOL_PREFIX}/bin/${PLATFORM_TRIPLET}-gcc" )
set( CMAKE_CXX_COMPILER         "${PLATFORM_TOOL_PREFIX}/bin/${PLATFORM_TRIPLET}-g++" )
set( CMAKE_C_FLAGS              "${PLATFORM_FLAGS} -msse3 -mfpmath=sse" CACHE STRING "" )
set( CMAKE_CXX_FLAGS            "${PLATFORM_FLAGS} -msse3 -mfpmath=sse" CACHE STRING "" )
