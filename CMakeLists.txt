cmake_minimum_required( VERSION 2.6 )

project( openzone C CXX )

if( WIN32 )

  include_directories(
      ../iconv/include
      ../libxml2/include
      ../SDL/include
      ../SDL_image/include
      ../ogg/include
      ../vorbis/include
      ../OpenAL/include
      ../freealut/include )

  link_directories(
      ../../../iconv/lib
      ../../../libxml2/lib
      ../../../SDL/lib
      ../../../SDL_image/lib
      ../../../SDL_mixer/lib
      ../../../ogg/lib
      ../../../vorbis/lib
      ../../../OpenAL/libs/Win32
      ../../../freealut/lib )

  link_libraries(
      iconv.lib
      libxml2.lib
      SDLmain.lib
      SDL.lib
      SDL_image.lib
      SDL_mixer.lib
      opengl32.lib
      glu32.lib
      openal32.lib
      alut.lib
      vorbisfile.lib )

  add_definitions( /W4 )

  # TODO: precompiled headers for VC++
  add_custom_target( precompile ALL )

else( WIN32 )

  # required libraries
  find_package( LibXml2 REQUIRED )
  find_package( PhysFS REQUIRED )
  find_package( SDL REQUIRED )
  find_package( SDL_image REQUIRED )
  find_package( OpenGL REQUIRED )
  find_package( GLU REQUIRED )
  find_package( OpenAL REQUIRED )

  find_file( ALUT_INCLUDE_DIR AL )
  find_library( ALUT_LIBRARY alut )
  find_file( OGG_INCLUDE_DIR ogg )
  find_library( OGG_LIBRARY ogg )
  find_file( VORBIS_INCLUDE_DIR vorbis )
  find_library( VORBIS_LIBRARY vorbisfile )

  set( CXX_FLAGS -I/usr/include/libxml2 -pipe -fno-rtti
    -Wall -Wextra -Winvalid-pch -Wnon-virtual-dtor -Woverloaded-virtual
#   flags for debug build
    -g3
#   flags for optimized build (for profiling)
#    -O3 -march=pentium4 -mtune=pentium4 -msse2 -mfpmath=sse -ffast-math -DNDEBUG
#    -Wdisabled-optimization -fprofile-generate
#   flags for release build (build and extensively test optimized build first to generate good
#   profiling info)
#    -O3 -march=pentium4 -mtune=pentium4 -msse2 -mfpmath=sse -ffast-math -DNDEBUG
#    -Wdisabled-optimization -fprofile-use
  )
  add_definitions( ${CXX_FLAGS} )
  link_libraries( xml2 SDL SDL_image GL GLU openal alut vorbisfile )

  install( FILES src/client/dark
           DESTINATION bin/dark
           PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
                       GROUP_READ GROUP_EXECUTE
                       WORLD_READ WORLD_EXECUTE )

  # precompiled header
  include_directories( . )

  add_custom_target( precompile ALL
    DEPENDS precompiled.h.gch )

  add_custom_command(
    OUTPUT precompiled.h.gch
    DEPENDS base precompiled.h
    COMMAND
      rm -f precompiled.h.gch &&
      ${CMAKE_CXX_COMPILER} ${CXX_FLAGS} precompiled.h )

endif( WIN32 )

subdirs( src )
