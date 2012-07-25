/*
 *  Main.cpp
 *
 *  Game initialization and main loop
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#include "precompiled.h"

#include "Main.h"

#include "Game.h"
#include "Audio.h"
#include "Render.h"

#ifdef WIN32
# include <direct.h>
# include <sys/types.h>
# include <sys/stat.h>
#else
# include <unistd.h>
# include <sys/stat.h>
#endif

namespace oz
{
namespace client
{

  Main main;

  void Main::shutdown()
  {
    logFile.println( "Shutdown {" );
    logFile.indent();

    if( initFlags & INIT_GAME_START ) {
      logFile.println( "Stopping Game {" );
      logFile.indent();
      game.stop();
      logFile.unindent();
      logFile.println( "}" );
    }
    if( initFlags & INIT_RENDER_INIT ) {
      logFile.print( "Shutting down Graphics ..." );
      logFile.indent();
      render.free();
      logFile.unindent();
      logFile.printEnd( " OK" );
    }
    if( initFlags & INIT_AUDIO ) {
      logFile.print( "Shutting down Audio ..." );
      logFile.indent();
      audio.free();
      logFile.unindent();
      logFile.printEnd( " OK" );
    }
    if( initFlags & INIT_GAME_INIT ) {
      logFile.println( "Shutting down Game {" );
      logFile.indent();
      game.free();
      logFile.unindent();
      logFile.println( "}" );
    }
    if( initFlags & INIT_SDL ) {
      logFile.print( "Shutting down SDL ..." );
      SDL_ShowCursor( true );
      SDLNet_Quit();
      SDL_Quit();
      logFile.printEnd( " OK" );
    }

    logFile.unindent();
    logFile.println( "}" );
    logFile.printlnETD( OZ_APP_NAME " finished at" );

    config.clear();
  }

  void Main::main()
  {
#ifdef WIN32
    const char *homeVar = getenv( "HOME" );
    String home = String( homeVar == null ? OZ_RC_DIR : homeVar + String( "\\" OZ_RC_DIR ) );

    struct _stat homeDirStat;
    if( _stat( home.cstr(), &homeDirStat ) ) {
      printf( "No resource dir found, creating '%s' ...", home.cstr() );

      if( _mkdir( home.cstr() ) ) {
        printf( " Failed\n" );
        return;
      }
      printf( " OK\n" );
    }
    home = home + "\\";
#else
    const char *homeVar = getenv( "HOME" );
    String home = String( homeVar == null ? OZ_RC_DIR "/" : homeVar + String( "/" OZ_RC_DIR "/" ) );

    struct stat homeDirStat;
    if( stat( home.cstr(), &homeDirStat ) ) {
      printf( "No resource dir found, creating '%s' ...", home.cstr() );

      if( mkdir( home.cstr(), S_IRUSR | S_IWUSR | S_IXUSR ) ) {
        printf( " Failed\n" );
        return;
      }
      printf( " OK\n" );
    }
#endif

#ifdef OZ_LOG_FILE
    String logPath = home + OZ_LOG_FILE;

    if( !logFile.init( logPath, true, "  " ) ) {
      printf( "Can't create/open log file '%s' for writing\n", logPath.cstr() );
      return;
    }
    logFile.println( "Log file '%s'", logPath.cstr() );
    printf( "Log file '%s'\n", logPath.cstr() );
#else
    logFile.init( null, true, "  " );
    logFile.println( "Log stream stdout ... OK" );
#endif

    logFile.printlnETD( OZ_APP_NAME " started at" );

    logFile.print( "Initializing SDL ..." );

    // Don't mess with screensaver. In X11 it only makes effect for windowed mode, in fullscreen
    // mode screensaver never starts anyway. Turning off screensaver has a side effect: if the game
    // crashes or exits forcibly, it remains turned off. Besides that, in X11 several programs
    // (e.g. IM clients like Pidgin, Kopete, Psi) detect user's inactivity based on screensaver's
    // counter, so they don't detect that you are away if the screensaver is screwed.
    if( config.get( "screen.leaveScreensaver", true ) ) {
      SDL_putenv( (char*) "SDL_VIDEO_ALLOW_SCREENSAVER=1" );
    }
    if( config.get( "screen.nvVSync", true ) ) {
      SDL_putenv( (char*) "__GL_SYNC_TO_VBLANK=1" );
    }
    if( SDL_Init( SDL_INIT_VIDEO ) || SDLNet_Init() ) {
      logFile.printEnd( " Failed" );
      return;
    }
    game.input.currKeys = SDL_GetKeyState( null );
    logFile.printEnd( " OK" );

    initFlags |= INIT_SDL;

    String configPath = home + OZ_CONFIG_FILE;
    config.load( configPath );

    const char *data = config.get( "data", "/usr/share/openzone" );

    logFile.print( "Going to working directory '%s' ...", data );

#ifdef WIN32
    if( _chdir( data ) != 0 ) {
      logFile.printEnd( " Failed" );
      return;
    }
    else {
      logFile.printEnd( " OK" );
    }
#else
    if( chdir( data ) != 0 ) {
      logFile.printEnd( " Failed" );
      return;
    }
    else {
      logFile.printEnd( " OK" );
    }
#endif

    int screenX    = config.get( "screen.width", 1024 );
    int screenY    = config.get( "screen.height", 768 );
    int screenBpp  = config.get( "screen.bpp", 32 );
    int screenFull = config.get( "screen.full", false ) ? SDL_FULLSCREEN : 0;

    Uint16 screenCenterX = (Uint16) ( screenX / 2 );
    Uint16 screenCenterY = (Uint16) ( screenY / 2 );

    logFile.print( "Setting OpenGL surface %dx%d %dbpp %s ...",
                   screenX, screenY, screenBpp, screenFull ? "fullscreen" : "windowed" );

    SDL_WM_SetCaption( OZ_WM_TITLE, null );
    SDL_ShowCursor( false );

    int modeResult = SDL_VideoModeOK( screenX, screenY, screenBpp, SDL_OPENGL | screenFull );
    if( modeResult == 0 ) {
      logFile.printEnd( " Mode not supported" );
      return;
    }
    if( SDL_SetVideoMode( screenX, screenY, screenBpp, SDL_OPENGL | screenFull ) == null ) {
      logFile.printEnd( " Failed" );
      return;
    }

    if( modeResult != screenBpp ) {
      logFile.printEnd( " OK, but at %dbpp", modeResult );
    }
    else {
      logFile.printEnd( " OK" );
    }

    initFlags |= INIT_SDL_VIDEO;

    logFile.println( "Initializing Graphics {" );
    logFile.indent();
    render.init();
    logFile.unindent();
    logFile.println( "}" );

    initFlags |= INIT_RENDER_INIT;

    logFile.println( "Initializing Audio {" );
    logFile.indent();

    if( !audio.init() ) {
      shutdown();
      return;
    }
//     audio.loadMusic( "music/04_fanatic-unreleased-rage.ogg" );

    logFile.unindent();
    logFile.println( "}" );
    initFlags |= INIT_AUDIO;

    logFile.println( "Initializing Game {" );
    logFile.indent();
    if( !game.init() ) {
      return;
    }
    logFile.unindent();
    logFile.println( "}" );
    initFlags |= INIT_GAME_INIT;

    logFile.println( "Loading Graphics {" );
    logFile.indent();
    render.load();
    logFile.unindent();
    logFile.println( "}" );
    initFlags |= INIT_RENDER_LOAD;

    logFile.println( "Starting Game {" );
    logFile.indent();
    game.start();
    logFile.unindent();
    logFile.println( "}" );
    initFlags |= INIT_GAME_START;

    logFile.println( "MAIN LOOP {" );
    logFile.indent();

    SDL_Event event;

    bool isAlive = true;
    bool isActive = true;
    int  nFrames = 0;

    Uint32 tick     = config.get( "tick", 20 );
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
      game.input.mouse.x = 0;
      game.input.mouse.y = 0;
      game.input.mouse.b = 0;
      aCopy( game.input.keys, game.input.currKeys, SDLK_LAST );

      while( SDL_PollEvent( &event ) ) {
        switch( event.type ) {
          case SDL_MOUSEMOTION: {
            game.input.mouse.x = -event.motion.xrel;
            game.input.mouse.y = -event.motion.yrel;

            SDL_WarpMouse( screenCenterX, screenCenterY );
            break;
          }
          case SDL_KEYDOWN: {
            game.input.keys[event.key.keysym.sym] |= SDL_PRESSED;

            if( event.key.keysym.sym == SDLK_F12 ) {
              SDL_WM_IconifyWindow();
              isActive = false;
            }
            break;
          }
          case SDL_MOUSEBUTTONDOWN: {
            game.input.mouse.b |= (char) event.button.button;
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
      isAlive &= game.update( tick );
      audio.update();

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

    logFile.println( "Printing config at exit {" );
    logFile.print( "%s", config.toString( "  " ).cstr() );
    logFile.println( "}" );
  }

}
}

int main( int, char*[] )
{
  try {
    oz::client::main.main();
  }
  catch( oz::Exception e ) {
    oz::logFile.resetIndent();
    oz::logFile.println();
    oz::logFile.println( "*** EXCEPTION: %s line %d", e.file, e.line );
    oz::logFile.println( "*** MESSAGE: %s", e.message );
    oz::logFile.println();

    if( oz::logFile.isFile() ) {
      printf( "*** EXCEPTION: %s line %d\n*** MESSAGE: %s\n\n", e.file, e.line, e.message );
    }
  }
  oz::client::main.shutdown();

  return 0;
}
