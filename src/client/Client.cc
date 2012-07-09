/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2012 Davorin Učakar
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
 */

/**
 * @file client/Client.cc
 *
 * Game initialisation and main loop.
 */

#include "stable.hh"

#include "client/Client.hh"

#include "BuildInfo.hh"

#include "client/Camera.hh"
#include "client/MenuStage.hh"
#include "client/GameStage.hh"
#include "client/Sound.hh"
#include "client/Render.hh"
#include "client/Loader.hh"
#include "client/NaCl.hh"
#include "client/NaClUpdater.hh"
#include "client/Window.hh"
#include "client/Input.hh"

#include <unistd.h>

#if defined( __native_client__ )
# include <ppapi/cpp/completion_callback.h>
# include <ppapi/cpp/core.h>
#elif defined( _WIN32 )
# undef WIN32_LEAN_AND_MEAN
# include <shlobj.h>
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
    OZ_MAIN_CALL( this, {
      sound.free();
    } )
  }
  if( initFlags & INIT_RENDER ) {
    OZ_MAIN_CALL( this, {
      render.free();
    } )
  }
  if( initFlags & INIT_CONTEXT ) {
    context.free();
  }
  if( initFlags & INIT_LIBRARY ) {
    library.free();
  }
  if( initFlags & INIT_LINGUA ) {
    lingua.free();
  }
  if( initFlags & INIT_WINDOW ) {
    input.free();
    window.free();
  }

  if( ( initFlags & ( INIT_CONFIG | INIT_MAIN_LOOP ) ) == INIT_MAIN_LOOP ) {
    String configDir = config["dir.config"].get( "" );

    if( !configDir.isEmpty() ) {
      File configFile( configDir + "/client.json" );

      config.exclude( "dir.config" );
      config.exclude( "dir.local" );
      // TODO
//       config.removeUnused();

#ifdef _WIN32
      config.save( &configFile, "\r\n" );
#else
      config.save( &configFile );
#endif
    }
  }

  config.clear( initFlags & INIT_CONFIG );

  if( initFlags & INIT_PHYSFS ) {
    PFile::free();
  }

#ifndef __native_client__
  if( initFlags & INIT_SDL ) {
    SDL_Quit();
  }
#endif

  if( initFlags & INIT_MAIN_LOOP ) {
    Alloc::printSummary();

    Log::print( OZ_APPLICATION_TITLE " " OZ_APPLICATION_VERSION " finished on " );
    Log::printTime( Time::local() );
    Log::printEnd();
  }
}

void Client::printUsage( const char* invocationName )
{
  Log::printRaw(
    "Usage:\n"
    "  %s [-v] [-l | -i <mission>] [-t <num>] [-p <prefix>]\n"
    "\n"
    "  -v            More verbose log output.\n"
    "  -l            Skip main menu and load the last autosaved state.\n"
    "  -i <mission>  Skip main menu and start mission <mission>.\n"
    "  -t <num>      Exit after <num> seconds (can be a floating-point number) and\n"
    "                use 42 as the random seed. Useful for benchmarking.\n"
    "  -p <prefix>   Sets data directory to '<prefix>/share/" OZ_APPLICATION_NAME "'.\n"
    "                Defaults to '%s'.\n"
    "\n",
    invocationName,
    OZ_INSTALL_PREFIX );
}

int Client::main( int argc, char** argv )
{
  initFlags = 0;

  String invocationName = File( argv[0] ).baseName();

  bool   doAutoload    = false;
  bool   isBenchmark   = false;
  float  benchmarkTime = 0.0f;
  String mission;

  optind = 1;
  int opt;
  while( ( opt = getopt( argc, argv, "vli:t:p:h?" ) ) >= 0 ) {
    switch( opt ) {
      case 'v': {
        Log::showVerbose = true;
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
        try {
          benchmarkTime = String::parseFloat( optarg );
        }
        catch( const String::ParseException& ) {
          printUsage( invocationName );
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
        printUsage( invocationName );
        return EXIT_FAILURE;
      }
    }
  }

#if defined( __native_client__ )

  mission = "test";

  File::init( File::TEMPORARY, 10*1024*1024 );

  String configDir = "/config/openzone";
  String localDir = "/local/share/openzone";
  String musicDir = "/music";

  File::mkdir( "/config" );
  File::mkdir( "/local" );
  File::mkdir( "/local/share" );

#elif defined( _WIN32 )

  char configRoot[MAX_PATH];
  char localRoot[MAX_PATH];
  char musicRoot[MAX_PATH];

  if( !SHGetSpecialFolderPath( null, configRoot, CSIDL_APPDATA, false ) ) {
    throw Exception( "Failed to obtain APPDATA directory" );
  }
  if( !SHGetSpecialFolderPath( null, localRoot, CSIDL_LOCAL_APPDATA, false ) ) {
    throw Exception( "Failed to obtain LOCAL_APPDATA directory" );
  }
  if( !SHGetSpecialFolderPath( null, musicRoot, CSIDL_MYMUSIC, false ) ) {
    throw Exception( "Failed to obtain MYMUSIC directory" );
  }

  String configDir = String::str( "%s\\" OZ_APPLICATION_NAME, configRoot );
  String localDir  = String::str( "%s\\" OZ_APPLICATION_NAME, localRoot );
  String musicDir  = musicDir;

#else

  const char* home       = SDL_getenv( "HOME" );
  const char* configRoot = SDL_getenv( "XDG_CONFIG_HOME" );
  const char* localRoot  = SDL_getenv( "XDG_LOCAL_HOME" );
  const char* musicRoot  = SDL_getenv( "XDG_MUSIC_DIR" );

  if( home == null ) {
    throw Exception( "Cannot determine user home directory from environment" );
  }

  String configDir = configRoot == null ?
                     String::str( "%s/.config/" OZ_APPLICATION_NAME, home ) :
                     String::str( "%s/" OZ_APPLICATION_NAME, configRoot );

  String localDir = localRoot == null ?
                    String::str( "%s/.local/share/" OZ_APPLICATION_NAME, home ) :
                    String::str( "%s/" OZ_APPLICATION_NAME, localRoot );

  String musicDir = musicRoot == null ? String::str( "%s/Music", home ) : String( musicRoot );

#endif

  if( File::mkdir( configDir ) ) {
    Log::println( "Profile directory '%s' created", configDir.cstr() );
  }

  if( File::mkdir( configDir + "/saves" ) ) {
    Log::println( "Directory for saved games '%s/saves' created", configDir.cstr() );
  }

  if( File::mkdir( configDir + "/screenshots" ) ) {
    Log::println( "Directory for screenshots '%s/screenshots' created", configDir.cstr() );
  }

  if( File::mkdir( localDir ) ) {
    Log::println( "Directory for per-user content '%s' created", localDir.cstr() );
  }

  if( Log::init( configDir + "/client.log", true ) ) {
    Log::println( "Log file '%s'", Log::logFile() );
  }

  Log::print( OZ_APPLICATION_TITLE " " OZ_APPLICATION_VERSION " started on " );
  Log::printTime( Time::local() );
  Log::printEnd();

  OZ_MAIN_CALL( this, {
    if( SDL_Init( SDL_INIT_NOPARACHUTE | SDL_INIT_VIDEO | SDL_INIT_JOYSTICK ) == 0 ) {
      _this->initFlags |= INIT_SDL;
    }
  } )
  if( !( initFlags & INIT_SDL ) ) {
    throw Exception( "Failed to initialise SDL: %s", SDL_GetError() );
  }

  PFile::init( File::TEMPORARY, 32*1024*1024 );
  initFlags |= INIT_PHYSFS;

  Log::verboseMode = true;
  Log::println( "Build details {" );
  Log::indent();
  Log::println( "Date:            %s", BuildInfo::TIME );
  Log::println( "Host system:     %s", BuildInfo::HOST_SYSTEM );
  Log::println( "Target system:   %s", BuildInfo::TARGET_SYSTEM );
  Log::println( "Build type:      %s", BuildInfo::BUILD_TYPE );
  Log::println( "Compiler:        %s", BuildInfo::COMPILER );
  Log::println( "Compiler flags:  %s", BuildInfo::CXX_FLAGS );
  Log::println( "Linker flags:    %s", BuildInfo::EXE_LINKER_FLAGS );
  Log::unindent();
  Log::println( "}" );
  Log::verboseMode = false;

  File configFile( configDir + "/client.json" );
  if( config.load( &configFile ) ) {
    Log::printEnd( "Configuration read from '%s'", configFile.path().cstr() );

    if( String::equals( config["_version"].get( "" ), OZ_APPLICATION_VERSION ) ) {
      initFlags |= INIT_CONFIG;
    }
    else {
      Log::println( "Invalid configuration file version, configuration will be cleaned and written "
                    "upon exit" );

      config.clear();
      config.setObject();
      config.add( "_version", OZ_APPLICATION_VERSION );
      config["_version"];
    }
  }
  else {
    Log::println( "No configuration file, default configuration will be used and written upon "
                  "exit" );

    config.setObject();
    config.add( "_version", OZ_APPLICATION_VERSION );
    config["_version"];
  }

  config.add( "dir.config", configDir );
  config.add( "dir.local", localDir );

  // tag variables as used
  config["dir.config"];
  config["dir.local"];

  window.init();
  initFlags |= INIT_WINDOW;

  input.init();

  String prefix  = config.include( "dir.prefix", OZ_INSTALL_PREFIX ).asString();
  String dataDir = prefix + "/share/" OZ_APPLICATION_NAME;

#ifdef __native_client__

  DArray<String> packages = naclUpdater.update();

#endif

  Log::println( "Content search path {" );
  Log::indent();

#ifdef __native_client__

  foreach( pkg, packages.citer() ) {
    File pkgFile( localDir + "/" + *pkg );

    if( PFile::mount( pkgFile.path(), null, true ) ) {
      Log::println( "%s", pkgFile.path().cstr() );
    }
    else {
      throw Exception( "Failed to mount '%s' on / in PhysicsFS", pkgFile.path().cstr() );
    }
  }

  packages.dealloc();

#else

  const char* userMusicPath = config.include( "dir.music", musicDir ).asString();

  if( PFile::mount( userMusicPath, "/music", true ) ) {
    Log::println( "%s [mounted on /music]", userMusicPath );
  }

  if( PFile::mount( localDir, null, true ) ) {
    Log::println( "%s", localDir.cstr() );

    DArray<File> list = File( localDir ).ls();

    foreach( file, list.citer() ) {
      if( file->hasExtension( "zip" ) || file->hasExtension( "7z" ) ) {
        if( !PFile::mount( file->path(), null, true ) ) {
          throw Exception( "Failed to mount '%s' on / in PhysicsFS", file->path().cstr() );
        }
        Log::println( "%s", file->path().cstr() );
      }
    }
  }

  if( PFile::mount( dataDir, null, true ) ) {
    Log::println( "%s", dataDir.cstr() );

    DArray<File> list = File( dataDir ).ls();

    foreach( file, list.citer() ) {
      if( file->hasExtension( "zip" ) || file->hasExtension( "7z" ) ) {
        if( !PFile::mount( file->path(), null, true ) ) {
          throw Exception( "Failed to mount '%s' on / in PhysicsFS", file->path().cstr() );
        }
        Log::println( "%s", file->path().cstr() );
      }
    }
  }

#endif

  Log::unindent();
  Log::println( "}" );

  config.include( "seed", "TIME" );

  if( config["seed"].type() == JSON::STRING && config["seed"].asString().equals( "TIME" ) ) {
    int seed = int( Time::uclock() );
    Math::seed( seed );
    Log::println( "Random generator seed set to the current time: %u", seed );
  }
  else {
    int seed = config["seed"].asInt();
    Math::seed( seed );
    Log::println( "Random generator seed set to: %d", seed );
  }

#ifdef __native_client__

  NaCl::post( "lang:" );

  for( String message = NaCl::poll(); ; message = NaCl::poll() ) {
    if( message.isEmpty() ) {
      Time::sleep( 10 );
    }
    else if( message.beginsWith( "lang:" ) ) {
      config.add( "lingua", message.substring( 5 ) );
      break;
    }
    else {
      NaCl::push( message );
    }
  }

#endif

  const char* locale = config.include( "lingua", "en" ).asString();

  Log::print( "Setting localisation '%s' ...", locale );
  if( lingua.init( locale ) ) {
    Log::printEnd( " OK" );
    initFlags |= INIT_LINGUA;
  }
  else {
    Log::printEnd( " Failed" );
  }

  initFlags |= INIT_LIBRARY;
  library.init( config["dir.music"].get( "" ) );

  initFlags |= INIT_CONTEXT;
  context.init();

  initFlags |= INIT_RENDER;
  OZ_MAIN_CALL( this, {
    render.init();
  } )
  render.swap();

  initFlags |= INIT_AUDIO;
  OZ_MAIN_CALL( this, {
    sound.init();
  } )

  initFlags |= INIT_STAGE_INIT;
  menuStage.init();
  gameStage.init();

#ifdef __native_client__
  NaCl::post( "none:" );
#endif

  Stage::nextStage = null;

  if( !mission.isEmpty() ) {
    gameStage.mission = mission;
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

  input.reset();

  SDL_Event event;

  bool isAlive        = true;
  bool isActive       = true;

  // time passed form start of the frame
  uint timeSpent;
  uint timeNow;
  uint timeZero       = Time::uclock();
  // time at start of the frame
  uint timeLast       = timeZero;
  uint timeLastRender = timeZero;

  initFlags |= INIT_MAIN_LOOP;

  Log::println( "Main loop {" );
  Log::indent();

  // THE MAGNIFICENT MAIN LOOP
  do {
    // read input & events
    input.prepare();

    while( SDL_PollEvent( &event ) != 0 ) {
      switch( event.type ) {
        case SDL_MOUSEBUTTONUP:
        case SDL_MOUSEBUTTONDOWN: {
          input.readEvent( &event );
          break;
        }
        case SDL_KEYDOWN: {
          input.readEvent( &event );

#ifndef __native_client__
          const SDL_keysym& keysym = event.key.keysym;

          if( keysym.sym == SDLK_F9 ) {
            if( keysym.mod == 0 ) {
              loader.makeScreenshot();
            }
          }
          else if( keysym.sym == SDLK_F11 ) {
            if( keysym.mod == 0 ) {
              window.toggleFull();
            }
            else if( keysym.mod & KMOD_CTRL ) {
              input.isLocked = !input.isLocked;

              SDL_ShowCursor( !input.isLocked );
            }
          }
          else if( keysym.sym == SDLK_F12 ) {
            if( keysym.mod == 0 ) {
              SDL_WM_IconifyWindow();
            }
            else if( keysym.mod & KMOD_CTRL ) {
              isAlive = false;
            }
          }
#endif
          break;
        }
        case SDL_ACTIVEEVENT: {
          if( event.active.state & SDL_APPMOUSEFOCUS ) {
            input.hasFocus = event.active.gain != 0;
          }
          if( event.active.state & SDL_APPACTIVE ) {
            if( event.active.gain ) {
              input.reset();

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
        case SDL_VIDEORESIZE: {
          window.width  = event.resize.w;
          window.height = event.resize.h;
          window.resize();
          break;
        }
        case SDL_QUIT: {
          isAlive = false;
          break;
        }
        default: {
          break;
        }
      }
    }

#ifdef __native_client__

    if( NaCl::width != window.width || NaCl::height != window.height ) {
      window.resize();
    }
    for( String message = NaCl::poll(); !message.isEmpty(); message = NaCl::poll() ) {
      if( message.equals( "quit:" ) ) {
        isAlive = false;
      }
    }

    input.keys[SDLK_LCTRL]     = 0;
    input.keys[SDLK_RCTRL]     = 0;
    input.keys[SDLK_RCTRL]     = 0;
    input.keys[SDLK_LALT]      = 0;
    input.keys[SDLK_RALT]      = 0;
    input.keys[SDLK_ESCAPE]    = 0;

    input.oldKeys[SDLK_LCTRL]  = 0;
    input.oldKeys[SDLK_RCTRL]  = 0;
    input.oldKeys[SDLK_RCTRL]  = 0;
    input.oldKeys[SDLK_LALT]   = 0;
    input.oldKeys[SDLK_RALT]   = 0;
    input.oldKeys[SDLK_ESCAPE] = 0;

#endif

    input.update();

    // Waste time when iconified.
    if( !isActive ) {
      Time::usleep( Timer::TICK_MICROS );

      timeSpent = Time::uclock() - timeLast;
      timeLast += timeSpent;

      continue;
    }

    timer.tick();

    isAlive &= !isBenchmark || timer.time < benchmarkTime;
    isAlive &= stage->update();

    if( Stage::nextStage != null ) {
      stage->unload();

      stage = Stage::nextStage;
      Stage::nextStage = null;

      input.prepare();
      input.update();

      stage->load();

      timeLast = Time::uclock();
      continue;
    }

    timeNow = Time::uclock();
    timeSpent = timeNow - timeLast;

    // Skip rendering graphics, only play sounds if there's not enough time.
    if( timeSpent >= Timer::TICK_MICROS && timeNow - timeLastRender < 1000 * 1000 ) {
      stage->present( false );
    }
    else {
      stage->present( true );

      timer.frame();
      // If there's still some time left, sleep.
      timeLastRender = Time::uclock();
      timeSpent = timeLastRender - timeLast;

      if( timeSpent < Timer::TICK_MICROS ) {
        stage->wait( Timer::TICK_MICROS - timeSpent );

        timeSpent = Timer::TICK_MICROS;
      }
    }

    if( timeSpent > 100 * 1000 ) {
      timer.drop( timeSpent - Timer::TICK_MICROS );
      timeLast += timeSpent - Timer::TICK_MICROS;
    }
    timeLast += Timer::TICK_MICROS;
  }
  while( isAlive );

  Log::unindent();
  Log::println( "}" );

  stage->unload();

  return EXIT_SUCCESS;
}

}
}
