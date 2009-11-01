cmake_minimum_required( VERSION 2.6 )

project( openzone C CXX )

  # required libraries
  find_package( LibXml2 REQUIRED )
  find_package( Lua51 REQUIRED )
  find_package( SDL REQUIRED )
  find_package( SDL_image REQUIRED )
  find_package( SDL_net REQUIRED )
  find_package( SDL_ttf REQUIRED )
  find_package( OpenGL REQUIRED )
  find_package( GLU REQUIRED )
  find_package( OpenAL REQUIRED )

  find_library( ALUT_LIBRARY alut )
  find_file( OGG_INCLUDE_DIR ogg )
  find_library( OGG_LIBRARY ogg )
  find_file( VORBIS_INCLUDE_DIR vorbis )
  find_library( VORBIS_LIBRARY vorbisfile )

  # generic C++ flags
  set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I/usr/include/libxml2 -I/usr/include/SDL -pipe -fno-rtti -std=gnu++0x" )
  # C++ warnings
  set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Winvalid-pch -Wnon-virtual-dtor -Woverloaded-virtual -Wdisabled-optimization" )
  # linker flags
  set( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lxml2 -llua -lSDL -lSDL_image -lSDL_net -lSDL_ttf -lGL -lGLU -lopenal -lalut -lvorbisfile" )

  install( FILES src/client/openzone
           DESTINATION bin/openzone
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
      ${CMAKE_CXX_COMPILER} `echo ${CMAKE_CXX_FLAGS}` precompiled.h )

add_subdirectory( src )
