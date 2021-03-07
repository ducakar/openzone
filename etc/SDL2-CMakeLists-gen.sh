cat << EOF > CMakeLists.txt
cmake_minimum_required( VERSION 2.8 )
project( SDL2 )

set( prefix "$buildDir/usr" )
set( exec_prefix "$buildDir/usr" )
set( libdir "$buildDir/usr/lib" )
set( includedir "$buildDir/usr/include" )
set( SDL_VERSION 2.0.0 )
set( SDL_LIBS -lSDL2 )
set( SDL_STATIC_LIBS -lSDL2 )
set( ENABLE_SHARED_TRUE "#" )
set( ENABLE_SHARED_FALSE "" )
set( ENABLE_STATIC_TRUE "" )
set( ENABLE_STATIC_FALSE "#" )
configure_file( sdl2.pc.in sdl2.pc @ONLY )
configure_file( sdl2-config.in sdl2-config )

include_directories( ./include )
add_definitions( -DANDROID -U__LINUX__ )

set( sources
  $(echo src/*.c \
        src/audio/*.c \
        src/audio/android/*.c \
        src/audio/dummy/*.c \
        src/atomic/SDL_atomic.c \
        src/atomic/SDL_spinlock.c \
        src/core/android/*.c \
        src/cpuinfo/*.c \
        src/events/*.c \
        src/file/*.c \
        src/haptic/*.c \
        src/haptic/dummy/*.c \
        src/joystick/*.c \
        src/joystick/android/*.c \
        src/loadso/dlopen/*.c \
        src/power/*.c \
        src/power/android/*.c \
        src/render/*.c \
        src/render/*/*.c \
        src/stdlib/*.c \
        src/thread/*.c \
        src/thread/pthread/*.c \
        src/timer/*.c \
        src/timer/unix/*.c \
        src/video/*.c \
        src/video/android/*.c \
        | sed -E 's| |\n  |g') )

add_library( SDL2 STATIC \${sources} )
set_target_properties( SDL2 PROPERTIES PUBLIC_HEADER "\${public_headers}" )
install( TARGETS SDL2 ARCHIVE DESTINATION lib LIBRARY DESTINATION lib )
install( DIRECTORY include/ DESTINATION include/SDL2 )
install( FILES \${CMAKE_CURRENT_BINARY_DIR}/sdl2.pc DESTINATION lib/pkgconfig )
install( PROGRAMS \${CMAKE_CURRENT_BINARY_DIR}/sdl2-config DESTINATION bin )

EOF

sed -E 's|Java_org_libsdl_app_|Java_com_github_ducakar_openzone_|g' -i \
    src/core/android/SDL_android.c \
    src/main/android/SDL_android_main.c
