set( ANDROID ON )

set( PLATFORM_TRIPLET           "arm-linux-androideabi" )
set( PLATFORM_PREFIX            "/opt/android-ndk/platforms/android-9/arch-arm/usr" )
set( PLATFORM_TOOL_PREFIX       "/opt/android-ndk/toolchains/arm-androideabi-4.6/prebuilt/linux-x86/bin" )
set( PLATFORM_SOURCE_PREFIX     "/opt/android-ndk/sources" )

set( CMAKE_SYSTEM_NAME          "Linux" CACHE STRING "Target system." )
set( CMAKE_SYSTEM_PROCESSOR     "ARM" CACHE STRING "Target processor." )
set( CMAKE_FIND_ROOT_PATH       "${PLATFORM_PREFIX}" )
set( CMAKE_C_COMPILER           "${PLATFORM_TOOL_PREFIX}/${PLATFORM_TRIPLET}-gcc" )
set( CMAKE_CXX_COMPILER         "${PLATFORM_TOOL_PREFIX}/${PLATFORM_TRIPLET}-g++" )
set( CMAKE_STRIP                "${PLATFORM_TOOL_PREFIX}/${PLATFORM_TRIPLET}-strip" )

set( PLATFORM_FLAGS             "--sysroot=${PLATFORM_PREFIX} -std=c++0x" )
set( PLATFORM_FLAGS             "${PLATFORM_FLAGS} -I${PLATFORM_SOURCE_PREFIX}/cxx-stl/gnu-libstdc++/4.6/include" )
set( PLATFORM_FLAGS             "${PLATFORM_FLAGS} -I${PLATFORM_SOURCE_PREFIX}/cxx-stl/gnu-libstdc++/4.6/libs/x86/include" )

set( CMAKE_C_FLAGS              "${PLATFORM_FLAGS}" CACHE STRING "" )
set( CMAKE_CXX_FLAGS            "${PLATFORM_FLAGS}" CACHE STRING "" )
