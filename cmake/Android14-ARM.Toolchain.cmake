set(ANDROID                     ON)

set(PLATFORM_EMBEDDED           ON)
set(PLATFORM_NAME               "Android")
set(PLATFORM_TRIPLET            "arm-linux-androideabi")
set(PLATFORM_PROC_PREFIX        "armeabi")
set(PLATFORM_PREFIX             "$ENV{ANDROID_NDK}/platforms/android-14/arch-arm")
set(PLATFORM_TOOL_PREFIX        "$ENV{ANDROID_NDK}/toolchains/arm-linux-androideabi-4.7/prebuilt/linux-x86_64")
set(PLATFORM_STL_PREFIX         "$ENV{ANDROID_NDK}/sources/cxx-stl/gnu-libstdc++/4.7")
set(PLATFORM_PORTS_PREFIX       "${CMAKE_SOURCE_DIR}/ports/Android14-ARM")
set(PLATFORM_FLAGS              "-fPIC -Wno-psabi --sysroot=${PLATFORM_PREFIX}")
set(PLATFORM_STL_LIBRARY        "${PLATFORM_STL_PREFIX}/libs/armeabi/libgnustl_static.a")

set(CMAKE_SYSTEM_NAME           "Linux" CACHE STRING "Target system.")
set(CMAKE_SYSTEM_PROCESSOR      "ARM" CACHE STRING "Target processor.")
set(CMAKE_FIND_ROOT_PATH        "${PLATFORM_PORTS_PREFIX};${PLATFORM_PREFIX}")
set(CMAKE_C_COMPILER            "${PLATFORM_TOOL_PREFIX}/bin/${PLATFORM_TRIPLET}-gcc")
set(CMAKE_CXX_COMPILER          "${PLATFORM_TOOL_PREFIX}/bin/${PLATFORM_TRIPLET}-g++")
set(CMAKE_C_FLAGS               "${PLATFORM_FLAGS}" CACHE STRING "")
set(CMAKE_CXX_FLAGS             "${PLATFORM_FLAGS}" CACHE STRING "")
set(CMAKE_C_FLAGS_RELEASE       "-Ofast" CACHE STRING "")
set(CMAKE_CXX_FLAGS_RELEASE     "-Ofast" CACHE STRING "")

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

include_directories(SYSTEM ${PLATFORM_STL_PREFIX}/include)
include_directories(SYSTEM ${PLATFORM_STL_PREFIX}/libs/armeabi/include)
