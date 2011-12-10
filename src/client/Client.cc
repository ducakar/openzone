/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 * Copyright (C) 2002-2011  Davorin Učakar
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Davorin Učakar
 * <davorin.ucakar@gmail.com>
 */

/**
 * @file client/Client.cc
 *
 * Game initialisation and main loop.
 */

#include "stable.hh"

#include "client/Client.hh"

#include "BuildInfo.hh"

#include "client/Context.hh"
#include "client/Camera.hh"
#include "client/MenuStage.hh"
#include "client/GameStage.hh"
#include "client/Sound.hh"
#include "client/Render.hh"
#include "client/Loader.hh"

#include <cerrno>
#include <clocale>
#include <ctime>
#include <unistd.h>

#ifdef OZ_NETWORK
# include <SDL_net.h>
#endif

namespace oz
{
namespace client
{

Client client;

void Client::shutdown()
{
  if( initFlags & INIT_STAGE_INIT ) {
    gameStage.free();
    menuStage.free();
  }
  if( initFlags & INIT_AUDIO ) {
    sound.free();
  }
  if( initFlags & INIT_RENDER_INIT ) {
    render.free();
  }
  if( initFlags & INIT_CONTEXT_INIT ) {
    context.free();
  }
  if( initFlags & INIT_LIBRARY ) {
    library.free();
  }
  if( ( initFlags & ( INIT_CONFIG | INIT_MAIN_LOOP ) ) == INIT_MAIN_LOOP ) {
    String rcDir = config.get( "dir.rc", "" );

    if( !rcDir.isEmpty() ) {
      String configPath = rcDir + "/" OZ_CLIENT_CONFIG_FILE;

      config.exclude( "dir.rc" );
      config.save( configPath );
    }
  }

  config.clear( initFlags & INIT_CONFIG );

  if( initFlags & INIT_SDL ) {
    log.print( "Shutting down SDL ..." );
    SDL_Quit();
    log.printEnd( " OK" );
  }

  if( initFlags & INIT_MAIN_LOOP ) {
    if( log.isVerbose ) {
      Alloc::printStatistics();
    }

    log.print( OZ_APPLICATION_TITLE " " OZ_APPLICATION_VERSION " finished on " );
    log.printTime();
    log.printEnd();
  }
}

void Client::printUsage()
{
  log.println( "Usage:" );
  log.indent();
  log.println( "openzone [-v] [-l | -i <function>] [-t <num>] [-p <prefix>]" );
  log.println();
  log.println( "-v" );
  log.println( "\tMore verbose log output." );
  log.println();
  log.println( "-l" );
  log.println( "\tSkip main menu and load autosaved state." );
  log.println( "\tAutosaved game state resides in ~/" OZ_RC_DIR "/autosave.ozState." );
  log.println();
  log.println( "-i <mission>" );
  log.println( "\tSkip main menu and start mission <mission>." );
  log.println( "\tMissions reside in <prefix>/share/openzone/lua/mission in files named" );
  log.println( "\t<mission>.lua files." );
  log.println();
  log.println( "-t <num>" );
  log.println( "\tExit after <num> seconds (can be a floating-point number) and use 42 as" );
  log.println( "\tthe random seed. For benchmarking purposes." );
  log.println();
  log.println( "-p <prefix>" );
  log.println( "\tSets data directory to <prefix>/share/openzone and locale directory to" );
  log.println( "\t<prefix>/share/locale." );
  log.println( "\tDefault: '%s'.", OZ_INSTALL_PREFIX );
  log.println();
  log.unindent();
}

int Client::main( int argc, char** argv )
{
  initFlags = 0;

  bool   doAutoload    = false;
  bool   isBenchmark   = false;
  float  benchmarkTime = 0.0f;
  String mission;

  optind = 1;
  int opt;
  while( ( opt = getopt( argc, argv, "vli:t:p:" ) ) != -1 ) {
    switch( opt ) {
      case 'v': {
        log.isVerbose = true;
        break;
      }
      case 'l': {
        doAutoload = true;
        break;
      }
      case 'i': {
        mission = optarg;
        break;
      }
      case 't': {
        errno = 0;
        char* end;
        benchmarkTime = strtof( optarg, &end );

        if( errno != 0 ) {
          printUsage();
          return EXIT_FAILURE;
        }

        config.add( "seed", "42" );
        isBenchmark = true;
        break;
      }
      case 'p': {
        config.add( "dir.prefix", optarg );
        break;
      }
      default: {
        log.println();
        printUsage();
        return EXIT_FAILURE;
      }
    }
  }

#ifdef _WIN32
  const char* homeVar = getenv( "APPDATA" );
#else
  const char* homeVar = getenv( "HOME" );
#endif
  if( homeVar == null ) {
    throw Exception( "Cannot determine user home directory from environment" );
  }

  String rcDir = homeVar + String( "/" OZ_RC_DIR );

  File rcDirFile( rcDir.cstr() );
  if( rcDirFile.getType() != File::DIRECTORY ) {
    printf( "No resource directory found, creating '%s' ...", rcDir.cstr() );

    if( !File::mkdir( rcDir.cstr(), 0700 ) ) {
      throw Exception( "Resource directory creation failed" );
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
      throw Exception( "Can't create/open log file '%s' for writing\n", logPath.cstr() );
    }

    printf( "Log file '%s'\n", logPath.cstr() );

    log.println( "OpenZone  Copyright (C) 2002-2011  Davorin Učakar\n"
        "This program comes with ABSOLUTELY NO WARRANTY.\n"
        "This is free software, and you are welcome to redistribute it\n"
        "under certain conditions; See COPYING file for details.\n" );
  }

  log.print( OZ_APPLICATION_TITLE " " OZ_APPLICATION_VERSION " started on " );
  log.printTime();
  log.printEnd();

  log.print( "Initialising SDL ..." );
  if( SDL_Init( SDL_INIT_NOPARACHUTE | SDL_INIT_VIDEO ) != 0 ) {
    throw Exception( "Failed to initialise SDL" );
  }
  log.printEnd( " OK" );
  initFlags |= INIT_SDL;

  log.println( "Build details {" );
  log.indent();

  log.println( "Date:            %s", BuildInfo::TIME );
  log.println( "Host system:     %s", BuildInfo::HOST_SYSTEM );
  log.println( "Target system:   %s", BuildInfo::TARGET_SYSTEM );
  log.println( "Build type:      %s", BuildInfo::BUILD_TYPE );
  log.println( "Compiler:        %s", BuildInfo::COMPILER );
  log.println( "Compiler flags:  %s", BuildInfo::CXX_FLAGS );
  log.println( "Linker flags:    %s", BuildInfo::EXE_LINKER_FLAGS );

  log.unindent();
  log.println( "}" );

  String configPath = rcDir + "/" OZ_CLIENT_CONFIG_FILE;
  if( config.load( configPath ) ) {
    log.printEnd( "Configuration read from '%s'", configPath.cstr() );

    if( String::equals( config.get( "_version", "" ), OZ_APPLICATION_VERSION ) ) {
      initFlags |= INIT_CONFIG;
    }
    else {
      log.println( "Invalid configuration version, default configuration will be used and written "
                   "on exit" );

      String prefixDir = config.get( "dir.prefix", OZ_INSTALL_PREFIX );
      String seed      = config.get( "seed", "TIME" );

      config.clear();
      config.add( "_version", OZ_APPLICATION_VERSION );
      config.get( "_version", "" );
      config.add( "dir.prefix", prefixDir );
      config.add( "seed", seed );
    }
  }
  else {
    log.println( "No configuration file, default configuration will be used and written on exit" );
    config.add( "_version", OZ_APPLICATION_VERSION );
    config.get( "_version", "" );
  }

  config.add( "dir.rc", rcDir );

  log.print( "Setting localisation ..." );

  // LANGUAGE environment variable is a nasty thing, overrides LC_MESSAGES
  SDL_putenv( const_cast<char*>( "LANGUAGE" ) );
  setlocale( LC_MESSAGES, config.getSet( "locale.messages", "" ) );

  bindtextdomain( OZ_APPLICATION_NAME, "../locale" );
  bind_textdomain_codeset( OZ_APPLICATION_NAME, "UTF-8" );
  textdomain( OZ_APPLICATION_NAME );

  log.printEnd( " LC_MESSAGES: %s ... OK", setlocale( LC_MESSAGES, null ) );

  if( String::equals( config.getSet( "seed", "TIME" ), "TIME" ) ) {
    int seed = int( time( null ) );
    Math::seed( seed );
    log.println( "Random generator seed set to the current time: %d", seed );
  }
  else {
    int seed = config.get( "seed", 0 );
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
  if( !File::chdir( dataDir ) ) {
    throw Exception( "Failed to change working directory to '%s'", dataDir.cstr() );
  }
  log.printEnd( " OK" );

  initFlags |= INIT_LIBRARY;
  library.init();

  initFlags |= INIT_CONTEXT_INIT;
  context.init();

  initFlags |= INIT_RENDER_INIT;
  render.init();

  initFlags |= INIT_AUDIO;
  sound.init();

  initFlags |= INIT_STAGE_INIT;
  menuStage.init();
  gameStage.init();

  Stage::nextStage = null;

  if( !mission.isEmpty() ) {
    gameStage.missionFile = mission;
    stage = &gameStage;
  }
  else if( doAutoload ) {
    gameStage.stateFile = GameStage::AUTOSAVE_FILE;
    stage = &gameStage;
  }
  else {
    stage = &menuStage;
  }

  stage->load();

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

  initFlags |= INIT_MAIN_LOOP;

  log.println( "Main loop {" );
  log.indent();

  // THE MAGNIFICENT MAIN LOOP
  do {
    // read input & events
    ui::keyboard.prepare();
    ui::mouse.prepare();

    if( ui::mouse.isGrabOn ) {
      SDL_GetRelativeMouseState( &ui::mouse.relX, &ui::mouse.relY );
      ui::mouse.relY = -ui::mouse.relY;
    }

    while( SDL_PollEvent( &event ) ) {
      switch( event.type ) {
        case SDL_MOUSEMOTION: {
          if( !ui::mouse.isGrabOn ) {
            ui::mouse.relX = -event.motion.xrel;
            ui::mouse.relY = +event.motion.yrel;
            SDL_WarpMouse( screenCentreX, screenCentreY );
          }
          break;
        }
        case SDL_KEYDOWN: {
          const SDL_keysym& keysym = event.key.keysym;

          ui::keyboard.keys[keysym.sym] |= SDL_PRESSED;

          if( keysym.sym == SDLK_F10 ) {
            if( keysym.mod == 0 ) {
              loader.makeScreenshot();
            }
          }
          else if( keysym.sym == SDLK_F11 ) {
            if( ( keysym.mod & KMOD_CTRL ) && !( keysym.mod & ~KMOD_CTRL ) ) {
              render.toggleFullscreen();
            }
          }
          else if( keysym.sym == SDLK_F12 ) {
            if( keysym.mod == 0 ) {
              SDL_WM_IconifyWindow();
            }
            else if( ( keysym.mod & KMOD_CTRL ) && !( keysym.mod & ~KMOD_CTRL ) ) {
              isAlive = false;
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

      continue;
    }

    ui::mouse.update();

    timer.tick();

    isAlive &= stage->update();

    if( Stage::nextStage != null ) {
      stage->unload();

      stage = Stage::nextStage;
      Stage::nextStage = null;

      stage->load();

      timeLast = SDL_GetTicks();
      continue;
    }

    timeNow = SDL_GetTicks();
    timeSpent = timeNow - timeLast;

    // render graphics, if we have enough time left
    if( timeSpent < uint( Timer::TICK_MILLIS ) ||
        timeNow - timeLastRender > 1000 )
    {
      stage->present();

      timer.frame();
      // if there's still some time left, waste it
      timeLastRender = SDL_GetTicks();
      timeSpent = timeLastRender - timeLast;

      if( timeSpent < uint( Timer::TICK_MILLIS ) ) {
        SDL_Delay( Timer::TICK_MILLIS - timeSpent );

        timeSpent = Timer::TICK_MILLIS;
      }
    }
    if( timeSpent > 100 ) {
      timer.drop( timeSpent - Timer::TICK_MILLIS );

      timeLast += timeSpent - Timer::TICK_MILLIS;
    }
    timeLast += Timer::TICK_MILLIS;

    if( isBenchmark && float( SDL_GetTicks() - timeZero ) >= benchmarkTime * 1000.0f ) {
      isAlive = false;
    }
  }
  while( isAlive );

  log.unindent();
  log.println( "}" );

  stage->unload();

  return EXIT_SUCCESS;
}

}
}
