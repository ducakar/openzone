cmake_minimum_required( VERSION 2.6 )

project( openzone C CXX )

    # required libraries
#    find_package( LibXml2 REQUIRED )
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
    
    set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Winvalid-pch -Wold-style-cast" )
    set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Woverloaded-virtual -Wnon-virtual-dtor -Wdisabled-optimization" )
    set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pipe -fno-rtti" )

    # linux-gnu
    #set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I/usr/include/libxml2 -I/usr/include/SDL" )
    #set( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -llua -lSDL -lSDL_image -lSDL_net -lSDL_ttf -lGL -lGLU -lopenal -lalut -lvorbisfile" )

    # mingw32
    set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I/home/davorin/Projects/openzone/mingw32/include -I/home/davorin/Projects/openzone/mingw32/include/SDL" )
    set( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -L/home/davorin/Projects/openzone/mingw32/lib -lmingw32 -lSDLmain -lSDL -lSDL_image -lSDL_ttf -lSDL_net -lopengl32 -lglu32 -lOpenAL32 -lalut -lvorbisfile -llua" )

    install( FILES src/client/openzone
             DESTINATION bin/openzone
             PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
                         GROUP_READ GROUP_EXECUTE
                         WORLD_READ WORLD_EXECUTE )

    # precompiled header
    include_directories( . )

    add_custom_target( precompile ALL
    DEPENDS precompiled.h.gch )

    add_custom_command( OUTPUT precompiled.h.gch
                        DEPENDS base precompiled.h
                        COMMAND 
                            rm -f precompiled.h.gch &&
                            ${CMAKE_CXX_COMPILER} `echo ${CMAKE_CXX_FLAGS}` precompiled.h )

add_subdirectory( src )
