/*
 *  Main.cpp
 *
 *  Game initialization and main loop
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#include "precompiled.hpp"

#include "Main.hpp"

#include "Client.hpp"
#include "SoundManager.hpp"
#include "Render.hpp"

#ifdef WIN32
#  include <direct.h>
#  include <sys/types.h>
#  include <sys/stat.h>
#else
#  include <unistd.h>
#  include <sys/stat.h>
#endif

namespace oz
{
namespace Client
{

  Main main;

  void Main::defaultConfig()
  {
    config.add( "data",                               "/usr/share/dark/data" );
    config.add( "tick",                               "20" );

    config.add( "screen.width",                       "1024" );
    config.add( "screen.height",                      "768" );
    config.add( "screen.bpp",                         "32" );
    config.add( "screen.nvVSync",                     "1" );
    config.add( "screen.full",                        "1" );

    config.add( "sound.volume.effects",               "1.0" );
    config.add( "sound.volume.music",                 "1.0" );

    // SDL_mixer
#ifdef __WIN32__
    config.add( "sound.sdl.device",                   "waveout" );
#else
    config.add( "sound.sdl.device",                   "alsa" );
#endif
    config.add( "sound.sdl.frequency",                "44100" );
    config.add( "sound.sdl.channels",                 "2" );
    config.add( "sound.sdl.chunkSize",                "512" );

    config.add( "input.mouse.xSens",                  "0.2" );
    config.add( "input.mouse.ySens",                  "0.2" );
    config.add( "input.keys.xSens",                   "0.2" );
    config.add( "input.keys.ySens",                   "0.2" );

    config.add( "render.perspective.angle",           "80.0" );
    config.add( "render.perspective.aspect",          "0.0" );
    config.add( "render.perspective.min",             "0.1" );
    config.add( "render.perspective.max",             "300.0" );

    config.add( "render.camera.smoothCoef",           "0.3" );

    config.add( "render.particleRadius",              "0.5" );
    config.add( "render.drawAABBs",                   "0" );
    config.add( "render.showAim",                     "0" );
    config.add( "render.blendHeaven",                 "1" );
  }

  void Main::shutdown()
  {
    if( initFlags & INIT_CLIENT_START ) {
      logFile.println( "Stopping Game {" );
      logFile.indent();
      client.stop();
      logFile.unindent();
      logFile.println( "}" );
    }
    if( initFlags & INIT_RENDER_INIT ) {
      logFile.print( "Shutting down Graphics ..." );
      logFile.indent();
      render.free();
      logFile.unindent();
      logFile.printRaw( " OK\n" );
    }
    if( initFlags & INIT_SOUND ) {
      logFile.print( "Shutting down Sound ..." );
      logFile.indent();
      soundManager.free();
      logFile.unindent();
      logFile.printRaw( " OK\n" );
    }
    if( initFlags & INIT_CLIENT_INIT ) {
      logFile.println( "Shutting down Game {" );
      logFile.indent();
      client.free();
      logFile.unindent();
      logFile.println( "}" );
    }
    if( initFlags & INIT_SDL ) {
      logFile.print( "Shutting down SDL ..." );
      SDL_ShowCursor( true );
      SDL_Quit();
      logFile.printRaw( " OK\n" );
    }
    logFile.printlnETD( "%s finished on", OZ_APP_NAME );
    config.clear();
  }

  void Main::main()
  {
#ifdef WIN32
    const char *homeVar = getenv( "HOME" );
    String home( homeVar == null ? OZ_RC_DIR : homeVar + String( "\\" OZ_RC_DIR ) );

    struct _stat homeDirStat;
    if( _stat( home.cstr(), &homeDirStat ) ) {
      printf( "No resource dir found, creating '%s' ...", home.cstr() );

      if( _mkdir( home.cstr() ) ) {
        printf( " Failed\n" );
        shutdown();
        return;
      }
      printf( " OK\n" );
    }
    home = home + "\\";
#else
    const char *homeVar = getenv( "HOME" );
    String home( homeVar == null ? OZ_RC_DIR "/" : homeVar + String( "/" OZ_RC_DIR "/" ) );

    struct stat homeDirStat;
    if( stat( home.cstr(), &homeDirStat ) ) {
      printf( "No resource dir found, creating '%s' ...", home.cstr() );

      if( mkdir( home.cstr(), S_IRUSR | S_IWUSR | S_IXUSR ) ) {
        printf( " Failed\n" );
        shutdown();
        return;
      }
      printf( " OK\n" );
    }
#endif

#ifdef OZ_LOG_FILE
    String logPath = home + OZ_LOG_FILE;

    if( !logFile.init( logPath, true, "  " ) ) {
      printf( "Can't create/open log file '%s' for writing\n", logPath.cstr() );
      shutdown();
      return;
    }
    logFile.println( "Log file '%s'", logPath.cstr() );
    printf( "Log file '%s'\n", logPath.cstr() );
#else
    logFile.init( null, true, "  " );
    logFile.println( "Log stream stdout ... OK" );
#endif

    logFile.printlnETD( "%s started on", OZ_APP_NAME );

    logFile.print( "Initializing SDL ..." );
    if( SDL_Init( SDL_INIT_VIDEO ) ) {
      logFile.printRaw( " Failed\n" );
      shutdown();
      return;
    }
    input.currKeys = SDL_GetKeyState( null );
    logFile.printRaw( " OK\n" );

    initFlags |= INIT_SDL;

    logFile.print( "Loading default config ..." );
    defaultConfig();
    logFile.printRaw( " OK\n" );

    const char *configPath = ( home + OZ_CONFIG_FILE ).cstr();

    if( !config.load( configPath ) ) {
      logFile.println( "Config not found, creating default {", configPath );
      logFile.indent();

      printf( "Config not found, creating default '%s' ...", configPath );

      if( !config.save( configPath ) ) {
        printf( " Failed\n" );
        shutdown();
        return;
      }
      printf( " OK\n" );

      logFile.unindent();
      logFile.println( "}" );
    }

    logFile.println( "Printing current config {" );
    logFile.print( "%s", config.toString( "  " ).cstr() );
    logFile.println( "}" );

    const char *data = config["data"].cstr();

    logFile.print( "Going to working directory '%s' ...", (const char*) data );

#ifdef WIN32
    if( _chdir( data ) != 0 ) {
      logFile.printRaw( " Failed\n" );
      shutdown();
      return;
    }
    else {
      logFile.printRaw( " OK\n" );
    }
#else
    if( chdir( data ) != 0 ) {
      logFile.printRaw( " Failed\n" );
      shutdown();
      return;
    }
    else {
      logFile.printRaw( " OK\n" );
    }
#endif

    int screenX    = atoi( config["screen.width"] );
    int screenY    = atoi( config["screen.height"] );
    int screenBpp  = atoi( config["screen.bpp"] );
    int screenFull = config["screen.full"] == "1" ? SDL_FULLSCREEN : 0;

    Uint16 screenCenterX = (Uint16) ( screenX / 2 );
    Uint16 screenCenterY = (Uint16) ( screenY / 2 );

    logFile.print( "Setting OpenGL surface %dx%d %dbpp %s ...",
                   screenX, screenY, screenBpp, screenFull ? "fullscreen" : "windowed" );

#ifndef WIN32
    if( config["screen.nvVSync"] == "1" ) {
      putenv( (char*) "__GL_SYNC_TO_VBLANK=1" );
    }
#endif
    SDL_WM_SetCaption( OZ_WM_TITLE, null );
    SDL_ShowCursor( false );

    int modeResult = SDL_VideoModeOK( screenX, screenY, screenBpp, SDL_OPENGL | screenFull );

    if( modeResult == 0 ) {
      logFile.printRaw( " Mode not supported\n" );
      shutdown();
      return;
    }
    if( SDL_SetVideoMode( screenX, screenY, screenBpp, SDL_OPENGL | screenFull ) == null ) {
      logFile.printRaw( " Failed\n" );
      shutdown();
      return;
    }

    if( modeResult != screenBpp ) {
      logFile.printRaw( " OK, but at %dbpp\n", modeResult );
    }
    else {
      logFile.printRaw( " OK\n" );
    }

    initFlags |= INIT_SDL_VIDEO;

    logFile.println( "Initializing Graphics {" );
    logFile.indent();
    render.init();
    logFile.unindent();
    logFile.println( "}" );

    initFlags |= INIT_RENDER_INIT;

    logFile.println( "Initializing Sound {" );
    logFile.indent();

    if( !soundManager.init() ) {
      shutdown();
      return;
    }
//     soundManager.loadMusic( "music/music.ogg" );

    logFile.unindent();
    logFile.println( "}" );
    initFlags |= INIT_SOUND;

    logFile.println( "Initializing Game {" );
    logFile.indent();
    if( !client.init() ) {
      shutdown();
      return;
    }
    logFile.unindent();
    logFile.println( "}" );
    initFlags |= INIT_CLIENT_INIT;

    logFile.println( "Loading Graphics {" );
    logFile.indent();
    render.load();
    logFile.unindent();
    logFile.println( "}" );
    initFlags |= INIT_RENDER_LOAD;

    logFile.println( "Starting Game {" );
    logFile.indent();
    client.start();
    logFile.unindent();
    logFile.println( "}" );
    initFlags |= INIT_CLIENT_START;

    logFile.println( "MAIN LOOP {" );
    logFile.indent();

    SDL_Event event;

    bool isAlive = true;
    bool isActive = true;
    int  nFrames = 0;

    Uint32 tick     = atoi( config["tick"] );
    // time passed form start of the frame
    Uint32 time;
    Uint32 timeZero = SDL_GetTicks();
    // time at start of the frame
    Uint32 timeLast = timeZero;

    // set mouse cursor to center of the screen and clear any events (key presses and mouse moves)
    // from before
    SDL_WarpMouse( screenCenterX, screenCenterY );
    while( SDL_PollEvent( &event ) ) {
    }

    // THE MAGNIFICANT MAIN LOOP
    do {
      // read input & events
      input.mouse.x = 0;
      input.mouse.y = 0;
      input.mouse.b = 0;
      aCopy( input.keys, input.currKeys, SDLK_LAST );

      while( SDL_PollEvent( &event ) ) {
        switch( event.type ) {
          case SDL_MOUSEMOTION: {
            input.mouse.x = -event.motion.xrel;
            input.mouse.y = -event.motion.yrel;

            SDL_WarpMouse( screenCenterX, screenCenterY );
            break;
          }
          case SDL_KEYDOWN: {
            input.keys[event.key.keysym.sym] |= SDL_PRESSED;

            if( event.key.keysym.sym == SDLK_F12 ) {
              SDL_WM_IconifyWindow();
              isActive = false;
            }
            break;
          }
          case SDL_MOUSEBUTTONDOWN: {
            input.mouse.b |= event.button.button;
            break;
          }
          case SDL_ACTIVEEVENT: {
            isActive |= event.active.gain && event.active.state == SDL_APPACTIVE;
            break;
          }
          case SDL_QUIT: {
            isAlive = false;
            break;
          }
        }
      }

      // waste time when iconified
      if( !isActive ) {
        time = SDL_GetTicks() - timeLast;

        if( time < tick ) {
          SDL_Delay( max( tick - time, 1u ) );
        }
        else if( time > 10 * tick ) {
          timeLast += time - tick;
        }
        timeLast += tick;
        continue;
      }

      // update world
      isAlive &= client.update( tick );
      soundManager.update();

      // render graphics, if we have enough time left
      time = SDL_GetTicks() - timeLast;

      if( time < tick ) {
        // render
        render.draw();
        nFrames++;

        // if there's still some time left, waste it
        time = SDL_GetTicks() - timeLast;

        if( time < tick ) {
          SDL_Delay( max( tick - time, 1u ) );
        }
      }
      else if( time > 10 * tick ) {
        timeLast += time - tick;
      }
      timeLast += tick;
    }
    while( isAlive );

    logFile.unindent();
    logFile.println( "}" );

    logFile.println( "Average framerate: %g",
                     (float) nFrames / (float) ( timeLast - timeZero ) * 1000.0f );
    shutdown();
  }

}
}

int main( int, char *[] )
{
  oz::Client::main.main();
  return 0;
}
