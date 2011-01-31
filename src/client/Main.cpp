/*
 *  Main.cpp
 *
 *  Game initialisation and main loop
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "client/Main.hpp"

#include "client/Context.hpp"
#include "client/GameStage.hpp"
#include "client/Sound.hpp"
#include "client/Render.hpp"

#include <ctime>
#ifdef OZ_MSVC
# include <direct.h>
#else
# include <unistd.h>
#endif
#include <sys/stat.h>
#include <SDL_net.h>

namespace oz
{
namespace client
{

  Main main;

  Main::Main() : allTime( 0.0f ), loadingTime( 0.0f )
  {}

  void Main::shutdown()
  {
    log.println( "Shutdown {" );
    log.indent();

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
      stage->free();
    }
    if( initFlags & INIT_SDL ) {
      log.print( "Shutting down SDL ..." );
      SDL_ShowCursor( SDL_TRUE );
      SDLNet_Quit();
      SDL_Quit();
      log.printEnd( " OK" );
    }

    config.clear();

    float loaderTime  = float( timer.loaderMillis )  * 0.001f;
    float syncTime    = float( timer.syncMillis )    * 0.001f;
    float renderTime  = float( timer.renderMillis )  * 0.001f;
    float sleepTime   = float( timer.sleepMillis )   * 0.001f;

    float matrixTime  = float( timer.matrixMillis )  * 0.001f;
    float nirvanaTime = float( timer.nirvanaMillis ) * 0.001f;

    log.println( "Time statistics {" );
    log.indent();
    log.println( "Loading time: %.2f s", loadingTime );
    log.println( "Matrix ticks: %d (%.2f Hz)", timer.ticks, float( timer.ticks ) / allTime );
    log.println( "Rendered frames: %d (%.2f Hz)", timer.nFrames, float( timer.nFrames ) / allTime );
    log.println( "Main loop time usage:" );
    log.println( "  %6.6g s  all time", allTime );
    log.println( "  %6.2f %%  [M:1  ] loader",                loaderTime  / allTime * 100.0f );
    log.println( "  %6.2f %%  [M:2.1] sync + sound.play",     syncTime    / allTime * 100.0f );
    log.println( "  %6.2f %%  [M:2.2] render + sound.update", renderTime  / allTime * 100.0f );
    log.println( "  %6.2f %%  [M:2.3] sleep",                 sleepTime   / allTime * 100.0f );
    log.println( "  %6.2f %%  [A:1  ] matrix",                matrixTime  / allTime * 100.0f );
    log.println( "  %6.2f %%  [A:2  ] nirvana",               nirvanaTime / allTime * 100.0f );
    log.unindent();
    log.println( "}" );

#ifdef OZ_ALLOC_STATISTICS
    log.println( "Heap usage (libraries not included) {" );
    log.println( "  current chunks     %d", Alloc::count  );
    log.println( "  current amount     %.2f MiB", float( Alloc::amount ) / ( 1024.0f*1024.0f ) );
    log.println( "  maximum chunks     %d", Alloc::maxCount );
    log.println( "  maximum amount     %.2f MiB", float( Alloc::maxAmount ) / ( 1024.0f*1024.0f ) );
    log.println( "  cumulative chunks  %d", Alloc::sumCount );
    log.println( "  cumulative amount  %.2f MiB", float( Alloc::sumAmount ) / ( 1024.0f*1024.0f ) );
    log.println( "}" );
#endif

    log.unindent();
    log.println( "}" );
    log.printlnETD( OZ_APP_NAME " finished at" );
  }

  void Main::main( int* argc, char** argv )
  {
    log.print( "Initialising SDL ..." );
    if( SDL_Init( SDL_INIT_NOPARACHUTE ) != 0 ) {
      log.printEnd( " Failed" );
      return;
    }
    log.printEnd( " OK" );

    uint createTime = SDL_GetTicks();

    initFlags = 0;
    String rcDir;

#ifdef OZ_WINDOWS
    const char* homeVar = getenv( "USERPROFILE" );
#else
    const char* homeVar = getenv( "HOME" );
#endif
    if( homeVar == null ) {
      throw Exception( "Cannot determine user home directory from environment" );
    }

    rcDir = homeVar + String( "/" OZ_RC_DIR );

    struct stat homeDirStat;
    if( stat( rcDir.cstr(), &homeDirStat ) != 0 ) {
      printf( "No resource directory found, creating '%s' ...", rcDir.cstr() );

#ifdef OZ_WINDOWS
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
    String logPath = rcDir + "/" OZ_LOG_FILE;

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
      log.println( "No configuration file, default configuration will be written on exit" );
    }
    config.add( "dir.rc", rcDir );

    if( config.contains( "seed" ) && config["seed"].equals( "time" ) ) {
      uint seed = uint( time( null ) );
      Math::seed( seed );
      log.println( "Random generator seed set to current time: %d", seed );
    }
    else {
      uint seed = config.getSet( "seed", 42 );
      Math::seed( seed );
      log.println( "Random generator seed set to: %d", seed );
    }

    log.print( "Initialising SDL subsystems ..." );

    // Don't mess with screensaver. In X11 it only makes effect for windowed mode, in fullscreen
    // mode screensaver never starts anyway. Turning off screensaver has a side effect: if the game
    // crashes, it remains turned off. Besides that, in X11 several programs (e.g. IM clients like
    // Pidgin, Kopete, Psi) rely on screensaver's counter, so they don't detect that you are away
    // if the screensaver is screwed.
    if( config.getSet( "screen.leaveScreensaver", true ) ) {
      SDL_putenv( const_cast<char*>( "SDL_VIDEO_ALLOW_SCREENSAVER=1" ) );
    }
    if( config.getSet( "screen.nvVSync", true ) ) {
      SDL_putenv( const_cast<char*>( "__GL_SYNC_TO_VBLANK=1" ) );
    }
    if( SDL_InitSubSystem( SDL_INIT_VIDEO ) != 0 || SDLNet_Init() != 0 ) {
      log.printEnd( " Failed" );
      return;
    }
    ui::keyboard.init();
    log.printEnd( " OK" );

    initFlags |= INIT_SDL;

    const char* data = config.getSet( "dir.data", OZ_DEFAULT_DATA_DIR );

    log.print( "Setting working directory to data directory '%s' ...", data );

    if( chdir( data ) != 0 ) {
      log.printEnd( " Failed" );
      return;
    }
    else {
      log.printEnd( " OK" );
    }

    int screenX    = config.get( "screen.width", 0 );
    int screenY    = config.get( "screen.height", 0 );
    int screenBpp  = config.get( "screen.bpp", 0 );
    int screenFull = config.getSet( "screen.full", false ) ? SDL_FULLSCREEN : 0;

    log.print( "Setting OpenGL surface %dx%d-%d %s ...",
               screenX, screenY, screenBpp, screenFull ? "fullscreen" : "windowed" );

    SDL_WM_SetCaption( OZ_WM_TITLE, null );
    SDL_ShowCursor( SDL_FALSE );

    if( ( screenX != 0 || screenY != 0 || screenBpp != 0 ) &&
        SDL_VideoModeOK( screenX, screenY, screenBpp, SDL_OPENGL | screenFull ) == 0 )
    {
      log.printEnd( " Mode not supported" );
      return;
    }
    SDL_Surface* surface = SDL_SetVideoMode( screenX, screenY, screenBpp, SDL_OPENGL | screenFull );
    if( surface == null ) {
      log.printEnd( " Failed" );
      return;
    }

    screenX   = surface->w;
    screenY   = surface->h;
    screenBpp = surface->format->BitsPerPixel;

    config.getSet( "screen.width", screenX );
    config.getSet( "screen.height", screenY );
    config.getSet( "screen.bpp", screenBpp );

    ushort screenCentreX = ushort( screenX / 2 );
    ushort screenCentreY = ushort( screenY / 2 );

    log.printEnd( " OK, %dx%d-%d set", screenX, screenY, screenBpp );

    initFlags |= INIT_SDL_VIDEO;

    render.init();
    initFlags |= INIT_RENDER_INIT;

    if( !sound.init( argc, argv ) ) {
      return;
    }
    initFlags |= INIT_AUDIO;

    stage = &gameStage;

    stage->init();
    stage->load();
    initFlags |= INIT_GAME_INIT;

    context.init();
    initFlags |= INIT_CONTEXT;

    render.load();
    initFlags |= INIT_RENDER_LOAD;

    loadingTime = float( SDL_GetTicks() - createTime ) / 1000.0f;

    SDL_Event event;

    // set mouse cursor to centre of the screen and clear any events (key presses and mouse moves)
    // from before
    SDL_WarpMouse( screenCentreX, screenCentreY );
    while( SDL_PollEvent( &event ) ) {
    }

    log.println( "Main loop {" );
    log.indent();

    bool isAlive        = true;
    bool isActive       = true;

    const uint tick     = static_cast<const uint>( Timer::TICK_MILLIS );
    // time passed form start of the frame
    uint delta;
    uint timeNow;
    uint timeZero       = SDL_GetTicks();
    // time at start of the frame
    uint timeLast       = timeZero;
    uint timeLastRender = timeZero;

    // THE MAGNIFICANT MAIN LOOP
    do {
      // read input & events
      ui::keyboard.prepare();
      ui::mouse.prepare();

      while( SDL_PollEvent( &event ) ) {
        switch( event.type ) {
          case SDL_MOUSEMOTION: {
            ui::mouse.relX = -event.motion.xrel;
            ui::mouse.relY =  event.motion.yrel;
            SDL_WarpMouse( screenCentreX, screenCentreY );
            break;
          }
          case SDL_KEYDOWN: {
            ui::keyboard.keys[event.key.keysym.sym] |= SDL_PRESSED;
            break;
          }
          case SDL_MOUSEBUTTONUP: {
            ui::mouse.currButtons &= char( ~SDL_BUTTON( event.button.button ) );
            break;
          }
          case SDL_MOUSEBUTTONDOWN: {
            ui::mouse.buttons |= char( SDL_BUTTON( event.button.button ) );
            ui::mouse.currButtons |= char( SDL_BUTTON( event.button.button ) );
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

      isAlive &= stage->update();

      timer.tick();
      timeNow = SDL_GetTicks();
      delta = timeNow - timeLast;

      // render graphics, if we have enough time left
      if( delta < tick || timeNow - timeLastRender > 32 * tick ) {
        stage->render();

        timer.frame();
        // if there's still some time left, waste it
        timeLastRender = SDL_GetTicks();
        delta = timeLastRender - timeLast;

        if( delta < tick ) {
          SDL_Delay( tick - delta );
          timer.sleepMillis += tick - delta;
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

    allTime = float( SDL_GetTicks() - timeZero ) / 1000.0f;

    if( ( initFlags & INIT_CONFIG ) == 0 ) {
      config.exclude( "dir.rc" );
      config.save( configPath );
      config.add( "dir.rc", rcDir );
    }
  }

}
}
