cat << EOF > CMakeLists.txt
cmake_minimum_required( VERSION 2.8 )
project( SDL2 )

include_directories( ./include )
add_definitions( -DANDROID -U__LINUX__ )

set( public_headers
  `echo include/*.h \
  | sed 's/ /\n  /g' | grep -v 'SDL_config_'` )
set( sources
  `echo src/*.c \
        src/audio/*.c \
        src/audio/android/*.c \
        src/audio/dummy/*.c \
        src/atomic/SDL_atomic.c \
        src/atomic/SDL_spinlock.c \
        src/core/android/*.cpp \
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
        | sed 's/ /\n  /g'` )

add_library( SDL2 STATIC \${sources} )
set_target_properties( SDL2 PROPERTIES PUBLIC_HEADER "\${public_headers}" )
install( TARGETS SDL2 ARCHIVE DESTINATION lib
                     LIBRARY DESTINATION lib
                     PUBLIC_HEADER DESTINATION include/SDL2 )
EOF
