set(PLATFORM_TRIPLET            "x86_64-w64-mingw32")
set(PLATFORM_PREFIX             "/usr/${PLATFORM_TRIPLET}")

set(CMAKE_SYSTEM_NAME           "Windows" CACHE STRING "Target system.")
set(CMAKE_SYSTEM_PROCESSOR      "x86_64" CACHE STRING "Target processor.")
set(CMAKE_FIND_ROOT_PATH        "${PLATFORM_PREFIX}")
set(CMAKE_AR                    "/usr/bin/${PLATFORM_TRIPLET}-ar" CACHE STRING "")
set(CMAKE_RANLIB                "/usr/bin/${PLATFORM_TRIPLET}-ranlib" CACHE STRING "")
set(CMAKE_RC_COMPILER           "/usr/bin/${PLATFORM_TRIPLET}-windres" CACHE STRING "")
set(CMAKE_STRIP                 "/usr/bin/${PLATFORM_TRIPLET}-strip" CACHE STRING "")
set(CMAKE_C_COMPILER            "/usr/bin/${PLATFORM_TRIPLET}-gcc")
set(CMAKE_CXX_COMPILER          "/usr/bin/${PLATFORM_TRIPLET}-g++")
set(CMAKE_C_FLAGS               "-march=x86-64-v2 -m64 -msse3" CACHE STRING "")
set(CMAKE_CXX_FLAGS             "-march=x86-64-v2 -m64 -msse3" CACHE STRING "")
# FIXME Enable "-flto" when fixed.
set(CMAKE_C_FLAGS_RELEASE       "-Ofast" CACHE STRING "")
set(CMAKE_CXX_FLAGS_RELEASE     "-Ofast" CACHE STRING "")

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

# CMake fails to detect this.
set(OPENAL_INCLUDE_DIR          "${CMAKE_FIND_ROOT_PATH}/include/AL")
