set( ANDROID ON )

set( PLATFORM_TRIPLET           "armv7a-android-linux" )
set( PLATFORM_PREFIX            "/opt/android-ndk/platforms/android-9/arch-armv7a/usr" )
set( PLATFORM_TOOL_PREFIX       "/opt/android-ndk/toolchains/armv7a-4.4.3/prebuilt/linux-x86/bin" )

set( CMAKE_SYSTEM_NAME          "Linux" CACHE STRING "Target system." )
set( CMAKE_SYSTEM_PROCESSOR     "ARM" CACHE STRING "Target processor." )
# set( CMAKE_FIND_ROOT_PATH       "${PLATFORM_PREFIX}" )
# set( CMAKE_C_COMPILER           "${PLATFORM_TOOL_PREFIX}/${PLATFORM_TRIPLET}-gcc" )
# set( CMAKE_CXX_COMPILER         "${PLATFORM_TOOL_PREFIX}/${PLATFORM_TRIPLET}-g++" )
# set( CMAKE_STRIP                "${PLATFORM_TOOL_PREFIX}/${PLATFORM_TRIPLET}-strip" )
