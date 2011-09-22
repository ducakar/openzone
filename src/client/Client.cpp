/*
 *  Client.cpp
 *
 *  Game initialisation and main loop
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "client/Client.hpp"

#include "Build.hpp"

#include "client/Context.hpp"
#include "client/Camera.hpp"
#include "client/GameStage.hpp"
#include "client/Sound.hpp"
#include "client/Render.hpp"

#include <cerrno>
#include <clocale>
#include <ctime>
#include <unistd.h>
#include <sys/stat.h>

#ifdef OZ_NETWORK
# include <SDL_net.h>
#endif

#ifdef OZ_MINGW
# define program_invocation_short_name "openzone"
#endif

namespace oz
{
namespace client
{

  Client client;

  void Client::shutdown()
  {
    uint beginTime = SDL_GetTicks();

#ifndef NDEBUG
    int nObjects  = 0;
    int nDynamics = 0;
    int nWeapons  = 0;
    int nBots     = 0;
    int nVehicles = 0;

    Vector< Pair<String, int> > classNameCounts;

    if( initFlags & INIT_MAIN_LOOP ) {
      log.print( "Counting objects ..." );

      Map<const ObjectClass*, int> classCounts;

      for( int i = 0; i < orbis.objects.length(); ++i ) {
        const Object* obj = orbis.objects[i];

        if( obj != null ) {
          int* count = classCounts.find( obj->clazz );

          if( count == null ) {
            classCounts.add( obj->clazz, 1 );
          }
          else {
            ++*count;
          }

          if( obj->flags & Object::VEHICLE_BIT ) {
            ++nVehicles;
          }
          else if( obj->flags & Object::BOT_BIT ) {
            ++nBots;
          }
          else if( obj->flags & Object::WEAPON_BIT ) {
            ++nWeapons;
          }
          else if( obj->flags & Object::DYNAMIC_BIT ) {
            ++nDynamics;
          }
          else {
            ++nObjects;
          }
        }
      }

      foreach( i, classCounts.citer() ) {
        classNameCounts.add( Pair<String, int>( i.key()->name, i.value() ) );
      }
      classCounts.clear();

      log.printEnd( " OK" );
    }
#endif

    if( initFlags & INIT_RENDER_LOAD ) {
      render.unload();
    }
    if( initFlags & INIT_CONTEXT_LOAD ) {
      context.unload();
    }
    if( initFlags & INIT_GAME_LOAD ) {
      stage->unload();
    }
    if( initFlags & INIT_GAME_INIT ) {
      stage->free();
    }
    if( initFlags & INIT_CONTEXT_INIT ) {
      context.free();
    }
    if( initFlags & INIT_AUDIO ) {
      sound.free();
    }
    if( initFlags & INIT_RENDER_INIT ) {
      render.free();
    }
    if( initFlags & INIT_TRANSLATOR ) {
      translator.free();
    }
    if( ( initFlags & ( INIT_CONFIG | INIT_MAIN_LOOP ) ) == INIT_MAIN_LOOP ) {
      String rcDir = config.get( "dir.rc", "" );

      if( !rcDir.isEmpty() ) {
        String configPath = rcDir + "/" OZ_CLIENT_CONFIG_FILE;

        config.exclude( "dir.rc" );
        config.save( configPath );
      }
    }

    config.clear();

    if( initFlags & INIT_SDL ) {
      log.print( "Shutting down SDL ..." );
      SDL_ShowCursor( SDL_TRUE );
      SDL_Quit();
      log.printEnd( " OK" );
    }

    if( ( initFlags & INIT_MAIN_LOOP ) && allTime >= Timer::TICK_TIME ) {
      float sleepTime     = float( timer.sleepMillis )           * 0.001f;
      float loaderTime    = float( timer.loaderMillis )          * 0.001f;
      float soundTime     = float( timer.soundMillis )           * 0.001f;
      float renderTime    = float( timer.renderMillis )          * 0.001f;
      float scheduleTime  = float( timer.renderScheduleMillis )  * 0.001f;
      float caelumTime    = float( timer.renderCaelumMillis )    * 0.001f;
      float terraTime     = float( timer.renderTerraMillis )     * 0.001f;
      float structsTime   = float( timer.renderStructsMillis )   * 0.001f;
      float objectsTime   = float( timer.renderObjectsMillis )   * 0.001f;
      float particlesTime = float( timer.renderParticlesMillis ) * 0.001f;
      float miscTime      = float( timer.renderMiscMillis )      * 0.001f;
      float renderUiTime  = float( timer.renderUiMillis )        * 0.001f;
      float syncTime      = float( timer.renderSyncMillis )      * 0.001f;
      float uiTime        = float( timer.uiMillis )              * 0.001f;

      float matrixTime    = float( timer.matrixMillis )          * 0.001f;
      float nirvanaTime   = float( timer.nirvanaMillis )         * 0.001f;

      float m2Time        = uiTime + soundTime + renderTime;

      float inactiveTime  = float( inactiveMillis )              * 0.001f;
      float droppedTime   = float( droppedMillis )               * 0.001f;
      float activeTime    = allTime - inactiveTime;

      int   frameDrops    = timer.ticks - timer.nFrames;

      float shutdownTime  = float( SDL_GetTicks() - beginTime )  * 0.001f;

#ifndef NDEBUG
      log.println( "Object counts {" );
      log.indent();

      foreach( i, classNameCounts.citer() ) {
        log.println( "%6d  %s", i->y, i->x.cstr() );
      }

      log.println();
      log.println( "%6d  vehicles", nVehicles );
      log.println( "%6d  bots", nBots );
      log.println( "%6d  weapons", nWeapons );
      log.println( "%6d  other dynamic objects", nDynamics );
      log.println( "%6d  static objects", nObjects );

      classNameCounts.clear();
      classNameCounts.dealloc();

      log.unindent();
      log.println( "}" );

      context.printLoad();
#endif

      log.println( "Memory usage {" );
      log.indent();

      Alloc::printStatistics();

      log.println( "Orbis                %.2f MiB (%d B)",
                   float( sizeof( Orbis ) ) / ( 1024.0f * 1024.0f ),
                   int( sizeof( Orbis ) ) );

      log.unindent();
      log.println( "}" );

      log.println( "Time statistics {" );
      log.indent();
      log.println( "Loading time             %.2f s",    loadingTime );
      log.println( "Shutdown time            %.2f s",    shutdownTime );
      log.println( "Main loop {" );
      log.println( "  Real time              %.2f s",    allTime );
      log.println( "  Active time            %.2f s",    activeTime );
      log.println( "  Inactive time          %.2f s",    inactiveTime );
      log.println( "  Dropped time           %.2f s",    droppedTime );
      log.println( "  Game time              %.2f s",    timer.time );
      log.println( "  Ticks in active time   %d (%.2f Hz)",
                  timer.ticks, float( timer.ticks ) / activeTime );
      log.println( "  Frames in active time  %d (%.2f Hz)",
                  timer.nFrames, float( timer.nFrames ) / activeTime );
      log.println( "  Frame drops            %d (%.2f %%)",
                  frameDrops, float( frameDrops ) / float( timer.ticks ) * 100.0f );
      log.println( "  Active time usage {" );
      log.println( "    %6.2f %%  [M:0] sleep",           sleepTime     / activeTime * 100.0f );
      log.println( "    %6.2f %%  [M:1] loader",          loaderTime    / activeTime * 100.0f );
      log.println( "    %6.2f %%  [M:2] presentation",    m2Time        / activeTime * 100.0f );
      log.println( "    %6.2f %%  [M:2] - sound",         soundTime     / activeTime * 100.0f );
      log.println( "    %6.2f %%  [M:2] - render",        renderTime    / activeTime * 100.0f );
      log.println( "    %6.2f %%  [M:2]   + schedule",    scheduleTime  / activeTime * 100.0f );
      log.println( "    %6.2f %%  [M:2]   + caelum",      caelumTime    / activeTime * 100.0f );
      log.println( "    %6.2f %%  [M:2]   + terra",       terraTime     / activeTime * 100.0f );
      log.println( "    %6.2f %%  [M:2]   + structs",     structsTime   / activeTime * 100.0f );
      log.println( "    %6.2f %%  [M:2]   + objects",     objectsTime   / activeTime * 100.0f );
      log.println( "    %6.2f %%  [M:2]   + particles",   particlesTime / activeTime * 100.0f );
      log.println( "    %6.2f %%  [M:2]   + misc",        miscTime      / activeTime * 100.0f );
      log.println( "    %6.2f %%  [M:2]   + ui",          renderUiTime  / activeTime * 100.0f );
      log.println( "    %6.2f %%  [M:2]   + sync",        syncTime      / activeTime * 100.0f );
      log.println( "    %6.2f %%  [M:3] ui",              uiTime        / activeTime * 100.0f );
      log.println( "    %6.2f %%  [A:1] matrix",          matrixTime    / activeTime * 100.0f );
      log.println( "    %6.2f %%  [A:2] nirvana",         nirvanaTime   / activeTime * 100.0f );
      log.println( "  }" );
      log.println( "}" );
      log.unindent();
      log.println( "}" );

      log.printlnETD( OZ_APPLICATION_TITLE " " OZ_APPLICATION_VERSION " finished at" );
    }
  }

  void Client::printUsage()
  {
    log.println( "Usage:" );
    log.indent();
    log.println( "openzone [--help] [--load | -l | --no-load | -L]" );
    log.println( "         [--save | -s | --no-save | -S] [(--time | -t) <num>]" );
    log.println( "         [(--prefix | -p) <prefix>]",
                 program_invocation_short_name );
    log.println();
    log.println( "--help" );
    log.println( "\tPrints that help message." );
    log.println();
    log.println( "-l, --load" );
    log.println( "-L, --no-load" );
    log.println( "\tEnables or disables autoload of ~/" OZ_RC_DIR "/default.ozState on" );
    log.println( "\tstartup respectively. Overrides the 'autoload' resource." );
    log.println();
    log.println( "-s, --save" );
    log.println( "-S, --no-save" );
    log.println( "\tEnables or disables autosave to ~/" OZ_RC_DIR "/default.ozState on exit" );
    log.println( "\trespectively. Overrides the 'autosave' resource." );
    log.println();
    log.println( "-t <num>, --time <num>" );
    log.println( "\tExits after <num> seconds (can be a floating-point number)." );
    log.println( "\tFor benchmarking purposes." );
    log.println();
    log.println( "-p <prefix>, --prefix <prefix>" );
    log.println( "\tSets data directory to <prefix>/share/openzone and locale directory to" );
    log.println( "\t<prefix>/share/locale." );
    log.println();
    log.unindent();
  }

  int Client::main( int argc, char** argv )
  {
    initFlags = 0;

    bool  isBenchmark = false;
    float benchmarkTime = 0.0f;

    for( int i = 1; i < argc; ++i ) {
      if( String::equals( argv[i], "--help" ) ) {
        printUsage();
        return -1;
      }
      else if( String::equals( argv[i], "--time" ) || String::equals( argv[i], "-t" ) ) {
        if( i + 1 == argc ) {
          printUsage();
          return -1;
        }

        errno = 0;
        char* end;

        benchmarkTime = strtof( argv[i + 1], &end );

        if( errno != 0 || end == argv[i + 1] ) {
          printUsage();
          return -1;
        }

        isBenchmark = true;
        ++i;
      }
      else if( String::equals( argv[i], "--load" ) || String::equals( argv[i], "-l" ) ) {
        config.add( "gameStage.autoload", "true" );
      }
      else if( String::equals( argv[i], "--no-load" ) || String::equals( argv[i], "-L" ) ) {
        config.add( "gameStage.autoload", "false" );
      }
      else if( String::equals( argv[i], "--save" ) || String::equals( argv[i], "-s" ) ) {
        config.add( "gameStage.autosave", "true" );
      }
      else if( String::equals( argv[i], "--no-save" ) || String::equals( argv[i], "-S" ) ) {
        config.add( "gameStage.autosave", "false" );
      }
      else if( String::equals( argv[i], "--prefix" ) || String::equals( argv[i], "-p" ) ) {
        if( i + 1 == argc ) {
          printUsage();
          return -1;
        }

        config.add( "dir.prefix", argv[i + 1] );
        ++i;
      }
      else {
        log.println( "Invalid command-line option '%s'", argv[i] );
        log.println();
        printUsage();
        return -1;
      }
    }

#ifdef OZ_MINGW
    const char* homeVar = getenv( "APPDATA" );
#else
    const char* homeVar = getenv( "HOME" );
#endif
    if( homeVar == null ) {
      throw Exception( "Cannot determine user home directory from environment" );
    }

    String rcDir = homeVar + String( "/" OZ_RC_DIR );

    struct stat homeDirStat;
    if( stat( rcDir.cstr(), &homeDirStat ) != 0 ) {
      printf( "No resource directory found, creating '%s' ...", rcDir.cstr() );

#ifdef OZ_MINGW
      if( mkdir( rcDir.cstr() ) != 0 ) {
#else
      if( mkdir( rcDir.cstr(), S_IRUSR | S_IWUSR | S_IXUSR ) != 0 ) {
#endif
        printf( " Failed\n" );
        return -1;
      }
      printf( " OK\n" );
    }

    if( String::equals( OZ_CLIENT_LOG_FILE, "STDOUT" ) ) {
      log.init( null, true, "  " );
      log.println( "Log stream stdout ... OK" );
    }
    else {
      String logPath = rcDir + "/" OZ_CLIENT_LOG_FILE;

      if( !log.init( logPath, true, "  " ) ) {
        printf( "Can't create/open log file '%s' for writing\n", logPath.cstr() );
        return -1;
      }

      printf( "Log file '%s'\n", logPath.cstr() );

      log.println( "OpenZone  Copyright (C) 2002-2011  Davorin Učakar\n"
          "This program comes with ABSOLUTELY NO WARRANTY.\n"
          "This is free software, and you are welcome to redistribute it\n"
          "under certain conditions; See COPYING file for details.\n" );
    }

    log.printlnETD( OZ_APPLICATION_TITLE " " OZ_APPLICATION_VERSION " started at" );

    log.print( "Initialising SDL ..." );
    if( SDL_Init( SDL_INIT_NOPARACHUTE | SDL_INIT_VIDEO ) != 0 ) {
      log.printEnd( " Failed" );
      return -1;
    }
    log.printEnd( " OK" );
    initFlags |= INIT_SDL;

    uint createTime = SDL_GetTicks();

    log.println( "Build details {" );
    log.indent();

    log.println( "Date:            %s", Build::TIME );
    log.println( "Host system:     %s", Build::HOST_SYSTEM );
    log.println( "Target system:   %s", Build::TARGET_SYSTEM );
    log.println( "Build type:      %s", Build::BUILD_TYPE );
    log.println( "Compiler:        %s", Build::COMPILER );
    log.println( "Compiler flags:  %s", Build::CXX_FLAGS );
    log.println( "Linker flags:    %s", Build::EXE_LINKER_FLAGS );

    log.unindent();
    log.println( "}" );

    String configPath = rcDir + "/" OZ_CLIENT_CONFIG_FILE;
    if( config.load( configPath ) ) {
      log.printEnd( "Configuration read from '%s'", configPath.cstr() );

      if( String::equals( config.get( "_version", "" ), OZ_APPLICATION_VERSION ) ) {
        initFlags |= INIT_CONFIG;
      }
      else {
        log.println( "Invalid configuration file version, will be overwritten on exit."
                     "If you experience any trouble, delete it." );
        config.add( "_version", OZ_APPLICATION_VERSION );
        config.add( "gameStage.autoload", "false" );
      }
    }
    else {
      log.println( "No configuration file, default configuration will be written on exit" );
      config.add( "_version", OZ_APPLICATION_VERSION );
    }

    config.add( "dir.rc", rcDir );

    log.print( "Setting localisation ..." );

    setlocale( LC_CTYPE, config.getSet( "locale.ctype", "" ) );
    setlocale( LC_MESSAGES, config.getSet( "locale.messages", "" ) );

    bindtextdomain( OZ_APPLICATION_NAME, "../locale" );
    bind_textdomain_codeset( OZ_APPLICATION_NAME, "UTF-8" );
    textdomain( OZ_APPLICATION_NAME );

    log.printEnd( " LC_CTYPE: %s LC_MESSAGES: %s ... OK",
                  setlocale( LC_CTYPE, null ),
                  setlocale( LC_MESSAGES, null ) );

    if( config.contains( "seed" ) && config["seed"].equals( "time" ) ) {
      int seed = int( time( null ) );
      Math::seed( seed );
      log.println( "Random generator seed set to current time: %d", seed );
    }
    else {
      int seed = config.getSet( "seed", 42 );
      Math::seed( seed );
      log.println( "Random generator seed set to: %d", seed );
    }

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
    ui::keyboard.init();

    String prefixDir = config.getSet( "dir.prefix", OZ_INSTALL_PREFIX );
    String dataDir   = prefixDir + "/share/" OZ_APPLICATION_NAME;

    log.print( "Setting working directory to data directory '%s' ...", dataDir.cstr() );
    if( chdir( dataDir ) != 0 ) {
      log.printEnd( " Failed" );
      return -1;
    }
    log.printEnd( " OK" );

    initFlags |= INIT_TRANSLATOR;
    translator.init();

    initFlags |= INIT_RENDER_INIT;
    render.init();

    initFlags |= INIT_AUDIO;
    sound.init();

    initFlags |= INIT_CONTEXT_INIT;
    context.init();

    stage = &gameStage;

    initFlags |= INIT_GAME_INIT;
    stage->init();

    initFlags |= INIT_GAME_LOAD;
    stage->load();

    initFlags |= INIT_CONTEXT_LOAD;
    context.load();

    render.draw( Render::DRAW_UI_BIT );
    render.sync();

    initFlags |= INIT_RENDER_LOAD;
    render.load();

    render.draw( Render::DRAW_UI_BIT );
    render.sync();

    stage->begin();

    SDL_Event event;

    // set mouse cursor to centre of the screen and clear any events (key presses and mouse moves)
    // from before
    ushort screenCentreX = ushort( camera.centreX );
    ushort screenCentreY = ushort( camera.centreY );

    SDL_WarpMouse( screenCentreX, screenCentreY );
    while( SDL_PollEvent( &event ) ) {
    }

    bool isAlive        = true;
    bool isActive       = true;

    // time passed form start of the frame
    uint timeSpent;
    uint timeNow;
    uint timeZero       = SDL_GetTicks();
    // time at start of the frame
    uint timeLast       = timeZero;
    uint timeLastRender = timeZero;

    loadingTime         = float( timeZero - createTime ) / 1000.0f;
    inactiveMillis      = 0;
    droppedMillis       = 0;

    initFlags |= INIT_MAIN_LOOP;

    log.println( "Main loop {" );
    log.indent();

    // THE MAGNIFICENT MAIN LOOP
    do {
      // read input & events
      ui::keyboard.prepare();
      ui::mouse.prepare();

      while( SDL_PollEvent( &event ) ) {
        switch( event.type ) {
          case SDL_MOUSEMOTION: {
            ui::mouse.relX = -event.motion.xrel;
            ui::mouse.relY = event.motion.yrel;
            SDL_WarpMouse( screenCentreX, screenCentreY );
            break;
          }
          case SDL_KEYDOWN: {
            const SDL_keysym& keysym = event.key.keysym;

            ui::keyboard.keys[keysym.sym] |= SDL_PRESSED;

            if( keysym.sym == SDLK_F12 ) {
              if( keysym.mod & KMOD_CTRL ) {
                isAlive = false;
              }
              else if( keysym.mod & KMOD_SHIFT ) {
                SDL_WM_ToggleFullScreen( render.surface );
              }
              else {
                SDL_WM_IconifyWindow();
              }
            }
            break;
          }
          case SDL_MOUSEBUTTONUP: {
            ui::mouse.currButtons &= char( ~SDL_BUTTON( event.button.button ) );
            break;
          }
          case SDL_MOUSEBUTTONDOWN: {
            ui::mouse.buttons |= char( SDL_BUTTON( event.button.button ) );
            ui::mouse.currButtons |= char( SDL_BUTTON( event.button.button ) );

            if( ui::mouse.buttons & SDL_BUTTON_WUMASK ) {
              ++ui::mouse.relZ;
            }
            if( ui::mouse.buttons & SDL_BUTTON_WDMASK ) {
              --ui::mouse.relZ;
            }
            break;
          }
          case SDL_ACTIVEEVENT: {
            if( event.active.state & SDL_APPACTIVE ) {
              if( event.active.gain ) {
                sound.resume();
                isActive = true;
              }
              else {
                sound.suspend();
                isActive = false;
              }
            }
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
        SDL_Delay( Timer::TICK_MILLIS );

        timeSpent = SDL_GetTicks() - timeLast;

        timeLast += timeSpent;
        inactiveMillis += timeSpent;

        continue;
      }

      ui::mouse.update();

      timer.tick();

      isAlive &= stage->update();

      timeNow = SDL_GetTicks();
      timeSpent = timeNow - timeLast;

      // render graphics, if we have enough time left
      if( timeSpent < uint( Timer::TICK_MILLIS ) ||
          timeNow - timeLastRender > uint( 50 * Timer::TICK_MILLIS ) )
      {
        stage->present();

        timer.frame();
        // if there's still some time left, waste it
        timeLastRender = SDL_GetTicks();
        timeSpent = timeLastRender - timeLast;

        if( timeSpent < uint( Timer::TICK_MILLIS ) ) {
          SDL_Delay( Timer::TICK_MILLIS - timeSpent );
          timer.sleepMillis += Timer::TICK_MILLIS - timeSpent;

          timeSpent = Timer::TICK_MILLIS;
        }
      }
      if( timeSpent > 4 * Timer::TICK_MILLIS ) {
        timeLast += timeSpent - Timer::TICK_MILLIS;
        droppedMillis += timeSpent - Timer::TICK_MILLIS;
      }
      timeLast += Timer::TICK_MILLIS;

      if( isBenchmark && float( SDL_GetTicks() - timeZero ) >= benchmarkTime * 1000.0f ) {
        isAlive = false;
      }
    }
    while( isAlive );

    log.unindent();
    log.println( "}" );

    allTime = float( SDL_GetTicks() - timeZero ) / 1000.0f;

    stage->end();

    return 0;
  }

}
}
