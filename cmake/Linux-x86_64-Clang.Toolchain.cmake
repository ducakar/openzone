set(CMAKE_SYSTEM_NAME              "Linux")
set(CMAKE_SYSTEM_PROCESSOR         "x86_64")
set(CMAKE_C_COMPILER               "/usr/bin/clang")
set(CMAKE_CXX_COMPILER             "/usr/bin/clang++")
set(CMAKE_C_FLAGS_INIT             "-march=x86-64-v2 -m64 -msse3")
set(CMAKE_CXX_FLAGS_INIT           "-march=x86-64-v2 -m64 -msse3")
set(CMAKE_EXE_LINKER_FLAGS_INIT    "-fuse-ld=lld")
set(CMAKE_MODULE_LINKER_FLAGS_INIT "-fuse-ld=lld")
set(CMAKE_SHARED_LINKER_FLAGS_INIT "-fuse-ld=lld")
