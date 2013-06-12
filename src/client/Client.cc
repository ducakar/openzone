/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2013 Davorin Učakar
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

#include <stable.hh>
#include <client/Client.hh>

#include <common/Timer.hh>
#include <common/Lua.hh>
#include <client/Camera.hh>
#include <client/MenuStage.hh>
#include <client/GameStage.hh>
#include <client/Sound.hh>
#include <client/Render.hh>
#include <client/Loader.hh>
#include <client/NaClUpdater.hh>
#include <client/Window.hh>
#include <client/Input.hh>
#include <client/Network.hh>
#include <client/BuildInfo.hh>

#include <clocale>
#include <unistd.h>
#include <SDL.h>
#include <SDL_ttf.h>

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

void Client::printUsage( const char* invocationName )
{
  Log::printRaw(
    "Usage:\n"
    "  %s [-v] [-l | -i <mission>] [-t <num>] [-L <lang>] [-p <prefix>]\n"
    "\n"
    "  -v            Print verbose log messages to terminal.\n"
    "  -l            Skip main menu and load the last autosaved game.\n"
    "  -i <mission>  Skip main menu and start mission <mission>.\n"
    "  -e <layout>   Edit world <layout> file. Create a new one if non-existent.\n"
    "  -t <num>      Exit after <num> seconds (can be a floating-point number) and\n"
    "                use 42 as the random seed. Useful for benchmarking.\n"
    "  -L <lang>     Use language <lang>. Should match a subdirectory name in\n"
    "                'lingua/' directory inside game data archives.\n"
    "  -p <prefix>   Sets data directory to '<prefix>/share/openzone'.\n"
    "                Defaults to '%s'.\n"
    "\n",
    invocationName, OZ_INSTALL_PREFIX );
}

int Client::init( int argc, char** argv )
{
  initFlags     = 0;
  isBenchmark   = false;
  benchmarkTime = 0.0f;

  String invocationName = String::fileBaseName( argv[0] );
  String prefix;
  String language;
  String mission;
  String layoutFile;
  bool   doAutoload = false;

  optind = 1;
  int opt;
  while( ( opt = getopt( argc, argv, "vli:e:t:L:p:h?" ) ) >= 0 ) {
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
      case 'e': {
        layoutFile = optarg;
        break;
      }
      case 't': {
        const char* end;
        benchmarkTime = float( String::parseDouble( optarg, &end ) );

        if( end == optarg ) {
          printUsage( invocationName );
          return EXIT_FAILURE;
        }

        isBenchmark = true;
        break;
      }
      case 'L': {
        language = optarg;
        break;
      }
      case 'p': {
        prefix = optarg;
        break;
      }
      default: {
        printUsage( invocationName );
        return EXIT_FAILURE;
      }
    }
  }

  File::init( File::TEMPORARY, 64*1024*1024 );

#if defined( __ANDROID__ )

  String configDir = OZ_ANDROID_ROOT "/config/openzone";
  String localDir  = OZ_ANDROID_ROOT "/local/share/openzone";
  String musicDir  = OZ_ANDROID_ROOT "/music";

  File::mkdir( OZ_ANDROID_ROOT "/config" );
  File::mkdir( OZ_ANDROID_ROOT "/local" );
  File::mkdir( OZ_ANDROID_ROOT "/local/share" );

#elif defined( __native_client__ )

  String configDir = "/config/openzone";
  String localDir  = "/local/share/openzone";
  String musicDir  = "/music";

  File::mkdir( "/config" );
  File::mkdir( "/local" );
  File::mkdir( "/local/share" );

#elif defined( _WIN32 )

  char configRoot[MAX_PATH];
  char localRoot[MAX_PATH];
  char musicRoot[MAX_PATH];

  if( !SHGetSpecialFolderPath( nullptr, configRoot, CSIDL_APPDATA, false ) ) {
    OZ_ERROR( "Failed to obtain APPDATA directory" );
  }
  if( !SHGetSpecialFolderPath( nullptr, localRoot, CSIDL_LOCAL_APPDATA, false ) ) {
    OZ_ERROR( "Failed to obtain LOCAL_APPDATA directory" );
  }
  if( !SHGetSpecialFolderPath( nullptr, musicRoot, CSIDL_MYMUSIC, false ) ) {
    OZ_ERROR( "Failed to obtain MYMUSIC directory" );
  }

  String configDir = String( configRoot, "\\openzone" );
  String localDir  = String( localRoot, "\\openzone" );
  String musicDir  = String( musicRoot, "\\OpenZone" );

#else

  const char* home       = SDL_getenv( "HOME" );
  const char* configRoot = SDL_getenv( "XDG_CONFIG_HOME" );
  const char* localRoot  = SDL_getenv( "XDG_LOCAL_HOME" );
  const char* musicRoot  = SDL_getenv( "XDG_MUSIC_DIR" );

  if( home == nullptr ) {
    OZ_ERROR( "Cannot determine user home directory from environment" );
  }

  String configDir = configRoot == nullptr ? String( home, "/.config/openzone" ) :
                                             String( configRoot, "/openzone" );

  String localDir = localRoot == nullptr ? String( home, "/.local/share/openzone" ) :
                                           String( localRoot, "/openzone" );

  String musicDir = musicRoot == nullptr ? String( home, "/Music/OpenZone" ) :
                                           String( musicRoot, "/OpenZone" );

#endif

  if( File::mkdir( configDir ) ) {
    Log::println( "Profile directory '%s' created", configDir.cstr() );
  }
  if( File::mkdir( configDir + "/saves" ) ) {
    Log::println( "Directory for saved games '%s/saves' created", configDir.cstr() );
  }
  if( File::mkdir( configDir + "/layouts" ) ) {
    Log::println( "Directory for layouts '%s/layouts' created", configDir.cstr() );
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

  Log::println( "OpenZone " OZ_VERSION " started on %s", Time::local().toString().cstr() );

  Log::verboseMode = true;
  Log::println( "Build details {" );
  Log::indent();
  Log::println( "Date:            %s", BuildInfo::TIME );
  Log::println( "Host:            %s", BuildInfo::HOST );
  Log::println( "Host arch:       %s", BuildInfo::HOST_ARCH );
  Log::println( "Target arch:     %s", BuildInfo::TARGET_ARCH );
  Log::println( "Build type:      %s", BuildInfo::BUILD_TYPE );
  Log::println( "Compiler:        %s", BuildInfo::COMPILER );
  Log::println( "Compiler flags:  %s", BuildInfo::CXX_FLAGS );
  Log::println( "Configuration:   %s", BuildInfo::CONFIG );
  Log::unindent();
  Log::println( "}" );
  Log::verboseMode = false;

  if( SDL_Init( SDL_INIT_NOPARACHUTE | SDL_INIT_VIDEO | SDL_INIT_JOYSTICK ) != 0 ) {
    OZ_ERROR( "Failed to initialise SDL: %s", SDL_GetError() );
  }
  initFlags |= INIT_SDL;

  if( TTF_Init() < 0 ) {
    OZ_ERROR( "Failed to initialise SDL_ttf" );
  }
  initFlags |= INIT_SDL_TTF;

  // Clean up after previous versions.
  File::rm( configDir + "/client.rc" );

  File configFile( configDir + "/client.json" );
  if( config.load( configFile ) ) {
    Log::printEnd( "Configuration read from '%s'", configFile.path().cstr() );

    if( String::equals( config["_version"].get( "" ), OZ_VERSION ) ) {
      initFlags |= INIT_CONFIG;
    }
    else {
      Log::println( "Invalid configuration file version, configuration will be cleaned and written"
                    " upon exit" );

      config = JSON( JSON::OBJECT );
      config.add( "_version", OZ_VERSION );
      config["_version"];
    }
  }
  else {
    Log::println( "No configuration file, default configuration will be used and written upon"
                  " exit" );

    config = JSON( JSON::OBJECT );
    config.add( "_version", OZ_VERSION );
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
  initFlags |= INIT_INPUT;

  network.init();
  initFlags |= INIT_NETWORK;

  config["dir.prefix"];

  if( prefix.isEmpty() ) {
    prefix = config.include( "dir.prefix", OZ_INSTALL_PREFIX ).asString();
  }

  String dataDir = prefix + "/share/openzone";

#ifdef __native_client__

  DArray<String> packages = naclUpdater.update();

#endif

  Log::println( "Content search path {" );
  Log::indent();

#ifdef __native_client__

  foreach( pkg, packages.citer() ) {
    File pkgFile( localDir + "/" + *pkg );

    if( File::mount( pkgFile.path(), nullptr, true ) ) {
      Log::println( "%s", pkgFile.path().cstr() );
    }
    else {
      OZ_ERROR( "Failed to mount '%s' on / in PhysicsFS: %s",
                pkgFile.path().cstr(), PHYSFS_getLastError() );
    }
  }

  packages.clear();

#else

  const char* userMusicPath = config.include( "dir.music", musicDir ).asString();

  if( File::mount( userMusicPath, "/userMusic", true ) ) {
    Log::println( "%s [mounted on /userMusic]", userMusicPath );
  }

  if( File::mount( localDir, nullptr, true ) ) {
    Log::println( "%s", localDir.cstr() );

    DArray<File> list = File( localDir ).ls();

    foreach( file, list.citer() ) {
      if( file->hasExtension( "7z" ) || file->hasExtension( "zip" ) ) {
        if( !File::mount( file->path(), nullptr, true ) ) {
          OZ_ERROR( "Failed to mount '%s' on / in PhysicsFS: %s",
                    file->path().cstr(), PHYSFS_getLastError() );
        }
        Log::println( "%s", file->path().cstr() );
      }
    }
  }

  if( File::mount( dataDir, nullptr, true ) ) {
    Log::println( "%s", dataDir.cstr() );

    DArray<File> list = File( dataDir ).ls();

    foreach( file, list.citer() ) {
      if( file->hasExtension( "7z" ) || file->hasExtension( "zip" ) ) {
        if( !File::mount( file->path(), nullptr, true ) ) {
          OZ_ERROR( "Failed to mount '%s' on / in PhysicsFS", file->path().cstr() );
        }
        Log::println( "%s", file->path().cstr() );
      }
    }
  }

#endif

  Log::unindent();
  Log::println( "}" );

#ifdef __native_client__

  Pepper::post( "lang:" );

  for( String message = Pepper::pop(); ; message = Pepper::pop() ) {
    if( message.isEmpty() ) {
      Time::sleep( 10 );
    }
    else if( message.beginsWith( "lang:" ) ) {
      language = message.substring( 5 );
      break;
    }
    else {
      Pepper::push( message );
    }
  }

#endif

  if( language.isEmpty() ) {
    language = config["lingua"].get( "" );
  }
  else {
    // Just tag as used.
    config["lingua"];
  }

  language = Lingua::detectLanguage( language );

  if( language.isEmpty() ) {
    language = "en";
    config.add( "lingua", language );
  }

  Log::print( "Setting language '%s' ...", language.cstr() );
  if( lingua.init( language ) ) {
    Log::printEnd( " OK" );

    initFlags |= INIT_LINGUA;
  }
  else {
    Log::printEnd( " Failed" );
  }

  config.include( "seed", "TIME" );

  int seed;

  if( config["seed"].type() == JSON::STRING ) {
    if( !config["seed"].asString().equals( "TIME" ) ) {
      OZ_ERROR( "Configuration variable 'sees' must be either \"TIME\" or an integer" );
    }

    seed = int( Time::time() );
  }
  else {
    seed = config["seed"].asInt();
    common::Lua::isRandomSeedTime = false;
  }

  if( isBenchmark ) {
    seed = 42;
    common::Lua::isRandomSeedTime = false;
  }

  Math::seed( seed );
  common::Lua::randomSeed = seed;

  Log::println( "Random generator seed set to: %d", seed );

  sound.initLibs();

  initFlags |= INIT_LIBRARY;
  liber.init( config["dir.music"].get( "" ) );

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
  Pepper::post( "none:" );
#endif

  Stage::nextStage = nullptr;

  if( !mission.isEmpty() || !layoutFile.isEmpty() ) {
    gameStage.layoutFile = File( layoutFile );
    gameStage.mission    = mission;

    stage = &gameStage;
  }
  else if( doAutoload ) {
    gameStage.stateFile = gameStage.autosaveFile;
    stage = &gameStage;
  }
  else {
    stage = &menuStage;
  }

  stage->load();

  window.setGrab( true );
  input.reset();

  return EXIT_SUCCESS;
}

void Client::shutdown()
{
  if( initFlags & INIT_STAGE_INIT ) {
    gameStage.destroy();
    menuStage.destroy();
  }
  if( initFlags & INIT_AUDIO ) {
    OZ_MAIN_CALL( this, {
      sound.destroy();
    } )
  }
  if( initFlags & INIT_RENDER ) {
    OZ_MAIN_CALL( this, {
      render.destroy();
    } )
  }
  if( initFlags & INIT_CONTEXT ) {
    context.destroy();
  }
  if( initFlags & INIT_LIBRARY ) {
    liber.destroy();
  }
  if( initFlags & INIT_LINGUA ) {
    lingua.destroy();
  }
  if( initFlags & INIT_NETWORK ) {
    network.destroy();
  }
  if( initFlags & INIT_INPUT ) {
    input.destroy();
  }
  if( initFlags & INIT_WINDOW ) {
    window.destroy();
  }
  if( ( initFlags & ( INIT_CONFIG | INIT_MAIN_LOOP ) ) == INIT_MAIN_LOOP ) {
    File configFile( config["dir.config"].asString() + "/client.json" );

    config.exclude( "dir.config" );
    config.exclude( "dir.local" );

    Log::print( "Writing configuration to '%s' ...", configFile.path().cstr() );

    config.save( configFile );

    Log::printEnd( " OK" );
  }

  config.clear( initFlags & INIT_CONFIG );

  if( initFlags & INIT_SDL_TTF ) {
    TTF_Quit();
  }
  if( initFlags & INIT_SDL ) {
    SDL_Quit();
  }

  File::destroy();

  if( initFlags & INIT_MAIN_LOOP ) {
    Log::printMemorySummary();
    Log::println( "OpenZone " OZ_VERSION " finished on %s", Time::local().toString().cstr() );
  }
}

int Client::main()
{
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

    SDL_PumpEvents();

    while( SDL_PollEvent( &event ) != 0 ) {
      switch( event.type ) {
        case SDL_KEYDOWN: {
#ifndef __native_client__
# if SDL_MAJOR_VERSION < 2
          const SDL_keysym& keysym = event.key.keysym;
# else
          const SDL_Keysym& keysym = event.key.keysym;
# endif

          if( keysym.sym == SDLK_F9 ) {
            if( !( keysym.mod & KMOD_CTRL ) ) {
              loader.makeScreenshot();
            }
            else {
              ui::ui.doShow = !ui::ui.doShow;
            }
          }
          else if( keysym.sym == SDLK_F11 ) {
            if( keysym.mod & KMOD_CTRL ) {
              window.setGrab( !window.hasGrab );
            }
            else  {
              window.setFullscreen( !window.isFull );
            }
          }
          else if( keysym.sym == SDLK_F12 ) {
            if( keysym.mod & KMOD_CTRL ) {
              isAlive = false;
            }
            else {
              window.minimise();
            }
          }
#endif
          input.readEvent( &event );
          break;
        }
        case SDL_KEYUP:
#if SDL_MAJOR_VERSION >= 2
        case SDL_MOUSEWHEEL:
#endif
        case SDL_MOUSEBUTTONUP:
        case SDL_MOUSEBUTTONDOWN: {
          input.readEvent( &event );
          break;
        }
#if defined( __native_client__ )
#elif SDL_MAJOR_VERSION < 2
        case SDL_ACTIVEEVENT: {
          if( event.active.state & SDL_APPMOUSEFOCUS ) {
            window.hasFocus = event.active.gain != 0;
          }
          if( event.active.state & SDL_APPACTIVE ) {
            if( event.active.gain ) {
              window.setGrab( window.hasGrab );
              input.reset();
              sound.resume();

              isActive = true;
            }
            else {
              sound.suspend();
              input.reset();

              isActive = false;
            }
          }
          break;
        }
#else
        case SDL_WINDOWEVENT: {
          switch( event.window.event ) {
            case SDL_WINDOWEVENT_FOCUS_GAINED:
            case SDL_WINDOWEVENT_ENTER: {
              window.hasFocus = true;
              input.reset();
              break;
            }
            case SDL_WINDOWEVENT_FOCUS_LOST:
            case SDL_WINDOWEVENT_LEAVE: {
              window.hasFocus = false;
              input.reset();
              break;
            }
            case SDL_WINDOWEVENT_RESTORED: {
              input.reset();
              sound.resume();

              isActive = true;
              break;
            }
            case SDL_WINDOWEVENT_MINIMIZED: {
              sound.suspend();
              input.reset();

              isActive = false;
              break;
            }
            case SDL_WINDOWEVENT_CLOSE: {
              isAlive = false;
              break;
            }
          }
          break;
        }
#endif
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

    if( Pepper::width != window.width || Pepper::height != window.height ) {
      window.resize();
    }
    if( window.hasFocus != Pepper::hasFocus ) {
      window.hasFocus = Pepper::hasFocus;
      input.reset();
    }
    for( String message = Pepper::pop(); !message.isEmpty(); message = Pepper::pop() ) {
      if( message.equals( "quit:" ) ) {
        isAlive = false;
      }
    }

    input.keys[SDLK_ESCAPE]    = false;
    input.oldKeys[SDLK_ESCAPE] = false;

#endif

    // Waste time when iconified.
    if( !isActive ) {
      Time::usleep( Timer::TICK_MICROS );

      timeSpent = Time::uclock() - timeLast;
      timeLast += timeSpent;

      continue;
    }

    input.update();

    timer.tick();

    isAlive &= !isBenchmark || timer.time < benchmarkTime;
    isAlive &= stage->update();

    if( Stage::nextStage != nullptr ) {
      stage->unload();

      stage = Stage::nextStage;
      Stage::nextStage = nullptr;

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

Client client;

}
}
