set( ANDROID ON )

set( PLATFORM_TRIPLET           "i686-android-linux" )
set( PLATFORM_PREFIX            "/opt/android-ndk/platforms/android-9/arch-x86/usr" )
set( PLATFORM_TOOL_PREFIX       "/opt/android-ndk/toolchains/x86-4.4.3/prebuilt/linux-x86/bin" )

set( CMAKE_SYSTEM_NAME          "Linux" CACHE STRING "Target system." )
set( CMAKE_SYSTEM_PROCESSOR     "i686" CACHE STRING "Target processor." )
# set( CMAKE_FIND_ROOT_PATH       "${PLATFORM_PREFIX}" )
# set( CMAKE_C_COMPILER           "${PLATFORM_TOOL_PREFIX}/${PLATFORM_TRIPLET}-gcc" )
# set( CMAKE_CXX_COMPILER         "${PLATFORM_TOOL_PREFIX}/${PLATFORM_TRIPLET}-g++" )
# set( CMAKE_STRIP                "${PLATFORM_TOOL_PREFIX}/${PLATFORM_TRIPLET}-strip" )

set( CMAKE_C_FLAGS              "-m32 -march=i686 -msse3 -mfpmath=sse" CACHE STRING "" )
set( CMAKE_CXX_FLAGS            "-m32 -march=i686 -msse3 -mfpmath=sse" CACHE STRING "" )
