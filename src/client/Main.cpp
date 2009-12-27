/*
 *  Main.cpp
 *
 *  Game initialization and main loop
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3.0. See COPYING for details.
 */

#include "precompiled.h"

#include "Main.h"

#include "Context.h"
#include "GameStage.h"
#include "Sound.h"
#include "Render.h"

#include <ctime>
#include <unistd.h>
#include <sys/stat.h>
#include <exception>

#include <SDL_net.h>

namespace oz
{
namespace client
{

  Main main;

  void Main::shutdown()
  {
    log.println( "Shutdown {" );
    log.indent();

    if( initFlags & INIT_GAME_START ) {
      stage->end();
    }
    if( initFlags & INIT_RENDER_INIT ) {
      render.unload();
      render.free();
    }
    if( initFlags & INIT_AUDIO ) {
      sound.free();
    }
    if( initFlags & INIT_CONTEXT ) {
      context.free();
    }
    if( initFlags & INIT_GAME_INIT ) {
      stage->unload();
    }
    if( initFlags & INIT_SDL ) {
      log.print( "Shutting down SDL ..." );
      SDL_ShowCursor( true );
      SDLNet_Quit();
      SDL_Quit();
      log.printEnd( " OK" );
    }

    if( Alloc::max != -1 ) {
      log.println( "Maximum allocated memory: %.2f MiB", Alloc::max / ( 1024.0f * 1024.0f ) );
    }

    log.unindent();
    log.println( "}" );
    log.printlnETD( OZ_APP_NAME " finished at" );

    config.clear();
  }

  void Main::main( int *argc, char **argv )
  {
    initFlags = 0;

    String rcDir;

#ifdef OZ_MINGW32
    const char *homeVar = getenv( "USERPROFILE" );
#else
    const char *homeVar = getenv( "HOME" );
#endif
    if( homeVar == null ) {
      throw Exception( "Cannot determine user home directory from environment" );
    }

    rcDir = homeVar + String( "/" OZ_RC_DIR );

    struct stat homeDirStat;
    if( stat( rcDir.cstr(), &homeDirStat ) != 0 ) {
      printf( "No resource dir found, creating '%s' ...", rcDir.cstr() );

#ifdef OZ_MINGW32
      if( mkdir( rcDir.cstr() ) != 0 ) {
#else
      if( mkdir( rcDir.cstr(), S_IRUSR | S_IWUSR | S_IXUSR ) != 0 ) {
#endif
        printf( " Failed\n" );
        return;
      }
      printf( " OK\n" );
    }

#ifdef OZ_LOG_FILE
    String logPath = rcDir + OZ_LOG_FILE;

    if( !log.init( logPath, true, "  " ) ) {
      printf( "Can't create/open log file '%s' for writing\n", logPath.cstr() );
      return;
    }
    log.println( "Log file '%s'", logPath.cstr() );
    printf( "Log file '%s'\n", logPath.cstr() );
#else
    log.init( null, true, "  " );
    log.println( "Log stream stdout ... OK" );
#endif

    log.printlnETD( OZ_APP_NAME " started at" );

    String configPath = rcDir + "/" OZ_CONFIG_FILE;
    if( config.load( configPath ) ) {
      log.printEnd( "Configuration read from '%s'", configPath.cstr() );
      initFlags |= INIT_CONFIG;
    }
    else {
      log.println( "No config file, default config will be written on exit" );
    }
    config.add( "dir.rc", rcDir );

    if( config.contains( "seed" ) && config["seed"].equals( "time" ) ) {
      uint seed = time( null );
      Math::seed( seed );
      log.println( "Random generator seed set to current time: %d", seed );
    }
    else {
      uint seed = config.get( "seed", 42 );
      Math::seed( seed );
      log.println( "Random generator seed set to: %d", seed );
    }

    log.print( "Initializing SDL ..." );

    // Don't mess with screensaver. In X11 it only makes effect for windowed mode, in fullscreen
    // mode screensaver never starts anyway. Turning off screensaver has a side effect: if the game
    // crashes or exits forcibly, it remains turned off. Besides that, in X11 several programs
    // (e.g. IM clients like Pidgin, Kopete, Psi) detect user's inactivity based on screensaver's
    // counter, so they don't detect that you are away if the screensaver is screwed.
    if( config.getSet( "screen.leaveScreensaver", true ) ) {
      SDL_putenv( const_cast<char*>( "SDL_VIDEO_ALLOW_SCREENSAVER=1" ) );
    }
    if( config.getSet( "screen.nvVSync", true ) ) {
      SDL_putenv( const_cast<char*>( "__GL_SYNC_TO_VBLANK=1" ) );
    }
    if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE ) || SDLNet_Init() ) {
      log.printEnd( " Failed" );
      return;
    }
    ui::keyboard.init();
    log.printEnd( " OK" );

    initFlags |= INIT_SDL;

    const char *data = config.getSet( "dir.data", "data" );

    log.print( "Setting working directory '%s' ...", data );

    if( chdir( data ) != 0 ) {
      log.printEnd( " Failed" );
      return;
    }
    else {
      log.printEnd( " OK" );
    }

    int screenX    = config.getSet( "screen.width", 1024 );
    int screenY    = config.getSet( "screen.height", 768 );
    int screenBpp  = config.getSet( "screen.bpp", 32 );
    int screenFull = config.getSet( "screen.full", false ) ? SDL_FULLSCREEN : 0;

    ushort screenCenterX = static_cast<ushort>( screenX / 2 );
    ushort screenCenterY = static_cast<ushort>( screenY / 2 );

    log.print( "Setting OpenGL surface %dx%d %dbpp %s ...",
               screenX, screenY, screenBpp, screenFull ? "fullscreen" : "windowed" );

    SDL_WM_SetCaption( OZ_WM_TITLE, null );
    SDL_ShowCursor( false );

    int modeResult = SDL_VideoModeOK( screenX, screenY, screenBpp, SDL_OPENGL | screenFull );
    if( modeResult == 0 ) {
      log.printEnd( " Mode not supported" );
      return;
    }
    if( SDL_SetVideoMode( screenX, screenY, screenBpp, SDL_OPENGL | screenFull ) == null ) {
      log.printEnd( " Failed" );
      return;
    }

    if( modeResult != screenBpp ) {
      log.printEnd( " OK, but at %dbpp", modeResult );
    }
    else {
      log.printEnd( " OK" );
    }
    initFlags |= INIT_SDL_VIDEO;

    render.init();
    initFlags |= INIT_RENDER_INIT;

    if( !sound.init( argc, argv ) ) {
      return;
    }
    initFlags |= INIT_AUDIO;

    stage = &gameStage;

    stage->load();
    initFlags |= INIT_GAME_INIT;

    context.init();
    initFlags |= INIT_CONTEXT;

    render.load();
    initFlags |= INIT_RENDER_LOAD;

    stage->begin();
    initFlags |= INIT_GAME_START;

    log.println( "MAIN LOOP {" );
    log.indent();

    SDL_Event event;

    bool isAlive        = true;
    bool isActive       = true;

    uint tick           = Timer::TICK_MILLIS;
    // time passed form start of the frame
    uint delta;
    uint timeNow;
    uint timeZero       = SDL_GetTicks();
    // time at start of the frame
    uint timeLast       = timeZero;
    uint timeLastRender = timeZero;

    uint gameTime       = 0;
    uint renderTime     = 0;

    timer.init();

    // set mouse cursor to center of the screen and clear any events (key presses and mouse moves)
    // from before
    SDL_WarpMouse( screenCenterX, screenCenterY );
    while( SDL_PollEvent( &event ) ) {
    }

    // THE MAGNIFICANT MAIN LOOP
    do {
      uint timeBegin = SDL_GetTicks();

      // read input & events
      ui::keyboard.prepare();
      ui::mouse.prepare();

      while( SDL_PollEvent( &event ) ) {
        switch( event.type ) {
          case SDL_MOUSEMOTION: {
            ui::mouse.relX = -event.motion.xrel;
            ui::mouse.relY =  event.motion.yrel;
            SDL_WarpMouse( screenCenterX, screenCenterY );
            break;
          }
          case SDL_KEYDOWN: {
            ui::keyboard.keys[event.key.keysym.sym] |= SDL_PRESSED;
            break;
          }
          case SDL_MOUSEBUTTONUP: {
            ui::mouse.currButtons &= ~SDL_BUTTON( event.button.button );
            break;
          }
          case SDL_MOUSEBUTTONDOWN: {
            ui::mouse.buttons |= SDL_BUTTON( event.button.button );
            ui::mouse.currButtons |= SDL_BUTTON( event.button.button );
            break;
          }
          case SDL_ACTIVEEVENT: {
            isActive |= ( event.active.gain && event.active.state == SDL_APPACTIVE );
            break;
          }
          case SDL_QUIT: {
            isAlive = false;
            break;
          }
        }
      }

      if( ui::keyboard.keys[SDLK_F12] ) {
        SDL_WM_IconifyWindow();
        isActive = false;
      }
      else if( ui::keyboard.keys[SDLK_F11] ) {
        render.doScreenshot = true;
      }

      // waste time when iconified
      if( !isActive ) {
        delta = SDL_GetTicks() - timeLast;
        timeLast += tick;

        if( delta < tick ) {
          SDL_Delay( tick - delta );
        }
        else if( delta > 16 * tick ) {
          timeLast = SDL_GetTicks() - tick;
        }
        continue;
      }

      ui::mouse.update();

      // stop nirvana, commit with cuts/removals, sync Render and Sound, update world,
      // resume nirvana
      isAlive &= stage->update();

      timer.tick();
      timeNow = SDL_GetTicks();
      delta = timeNow - timeLast;
      gameTime += timeNow - timeBegin;

      // render graphics, if we have enough time left
      if( delta < tick || timeNow - timeLastRender > 32 * tick ) {
        stage->render();

        timer.frame();
        // if there's still some time left, waste it
        timeLastRender = SDL_GetTicks();
        renderTime += timeLastRender - timeNow;
        delta = timeLastRender - timeLast;

        if( delta < tick ) {
          SDL_Delay( tick - delta );
        }
      }
      if( delta > 4 * tick ) {
        timeLast += delta - tick;
      }
      timeLast += tick;
    }
    while( isAlive );

    log.unindent();
    log.println( "}" );

    float allTimeSec = static_cast<float>( timeLast - timeZero ) / 1000.0f;
    float gameTimeSec = static_cast<float>( gameTime ) / 1000.0f;
    float renderTimeSec = static_cast<float>( renderTime ) / 1000.0f;
    float sleepTimeSec = max( 0.0f, allTimeSec - gameTimeSec - renderTimeSec );
    float nirvanaTimeSec = static_cast<float>( timer.nirvanaMillis ) / 1000.0f;

    log.println( "STATISTICS {" );
    log.indent();
    log.println( "Ticks: %d (%.2f Hz)", timer.millis / Timer::TICK_MILLIS, 1000.0f / Timer::TICK_MILLIS );
    log.println( "Frames: %d (%.2f Hz)", timer.nFrames, timer.nFrames / timer.time );
    log.println( "Time usage:" );
    log.println( "    %.4g s\t%.1f%%\tall time", allTimeSec, 100.0f );
    log.println( "    %.4g s\t%.1f%%\tsystem + simulation + basic sound update",
                 gameTimeSec, gameTimeSec / allTimeSec * 100.0f );
    log.println( "    %.4g s\t%.1f%%\trender + advanced sound update",
                 renderTimeSec, renderTimeSec / allTimeSec * 100.0f );
    log.println( "    %.4g s\t%.1f%%\tsleep", sleepTimeSec, sleepTimeSec / allTimeSec * 100.0f );
    log.println( "    %.4g s\t%.1f%%\t[own thread] artificial intelligence",
                 nirvanaTimeSec, nirvanaTimeSec / allTimeSec * 100.0f );
    log.unindent();
    log.println( "}" );

    if( ~initFlags & INIT_CONFIG ) {
      config.remove( "dir.rc" );
      config.save( configPath );
      config.add( "dir.rc", rcDir );
    }
  }

}
}

int main( int argc, char **argv )
{
  printf( "OpenZone  Copyright (C) 2002-2009  Davorin Učakar\n"
      "This program comes with ABSOLUTELY NO WARRANTY.\n"
      "This is free software, and you are welcome to redistribute it\n"
      "under certain conditions; See COPYING for details.\n\n" );

  try {
    oz::client::main.main( &argc, argv );
  }
  catch( const oz::Exception &e ) {
    oz::log.resetIndent();
    oz::log.println();
    oz::log.println( "EXCEPTION: %s:%d: %s", e.file, e.line, e.message );

    if( oz::log.isFile() ) {
      fprintf( stderr, "EXCEPTION: %s:%d: %s\n", e.file, e.line, e.message );
    }
  }
  catch( const std::exception &e ) {
    oz::log.resetIndent();
    oz::log.println();
    oz::log.println( "EXCEPTION: %s", e.what() );

    if( oz::log.isFile() ) {
      fprintf( stderr, "EXCEPTION: %s", e.what() );
    }
  }
  oz::client::main.shutdown();

  return 0;
}
