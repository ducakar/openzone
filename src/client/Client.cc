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
#include "client/NaClMainCall.hh"
#include "client/NaClGLContext.hh"

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
    sound.free();
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

  if( ( initFlags & ( INIT_CONFIG | INIT_MAIN_LOOP ) ) == INIT_MAIN_LOOP ) {
    String configDir = config.get( "dir.config", "" );

    if( !configDir.isEmpty() ) {
      File configFile( configDir + "/client.rc" );

      config.exclude( "dir.config" );
      config.exclude( "dir.local" );
      config.removeUnused();

#ifdef _WIN32
      config.save( configFile, "\r\n" );
#else
      config.save( configFile );
#endif
    }
  }

  config.clear( initFlags & INIT_CONFIG );

  if( initFlags & INIT_PHYSFS ) {
    PhysFile::free();
  }

  if( initFlags & INIT_SDL ) {
    OZ_MAIN_CALL( this, {
      SDL_Quit();
    } )
  }

  if( initFlags & INIT_MAIN_LOOP ) {
    Alloc::printSummary();

    Log::print( OZ_APPLICATION_TITLE " " OZ_APPLICATION_VERSION " finished on " );
    Log::printTime();
    Log::printEnd();
  }
}

void Client::printUsage()
{
  Log::println( "Usage:" );
  Log::indent();
  Log::println( OZ_APPLICATION_NAME " [-v] [-l | -i <mission>] [-t <num>] [-p <prefix>]" );
  Log::println();
  Log::println( "-v" );
  Log::println( "\tMore verbose log output." );
  Log::println();
  Log::println( "-l" );
  Log::println( "\tSkip main menu and load the last autosaved state." );
  Log::println();
  Log::println( "-i <mission>" );
  Log::println( "\tSkip main menu and start mission <mission>." );
  Log::println();
  Log::println( "-t <num>" );
  Log::println( "\tExit after <num> seconds (can be a floating-point number) and use 42 as" );
  Log::println( "\tthe random seed. Useful for benchmarking." );
  Log::println();
  Log::println( "-p <prefix>" );
  Log::println( "\tSets data directory to '<prefix>/share/" OZ_APPLICATION_NAME "'." );
  Log::println( "\tDefault: '%s'.", OZ_INSTALL_PREFIX );
  Log::println();
  Log::unindent();
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
        Log::println();
        printUsage();
        return EXIT_FAILURE;
      }
    }
  }

#if defined( __native_client__ )

  File::init( File::TEMPORARY, 10*1024*1024 );

  String configDir = "/config";
  String localDir = "/data";

#elif defined( _WIN32 )

  char configRoot[MAX_PATH];
  char localRoot[MAX_PATH];

  if( !SHGetSpecialFolderPath( null, configRoot, CSIDL_APPDATA, false ) ) {
    throw Exception( "Failed to access APPDATA directory" );
  }
  if( !SHGetSpecialFolderPath( null, localRoot, CSIDL_LOCAL_APPDATA, false ) ) {
    throw Exception( "Failed to access LOCAL_APPDATA directory" );
  }

  String configDir = String::str( "%s\\" OZ_APPLICATION_NAME, configRoot );
  String localDir  = String::str( "%s\\" OZ_APPLICATION_NAME, localRoot );

#else

  const char* home       = SDL_getenv( "HOME" );
  const char* configRoot = SDL_getenv( "XDG_CONFIG_HOME" );
  const char* localRoot  = SDL_getenv( "XDG_LOCAL_HOME" );

  if( home == null ) {
    throw Exception( "Cannot determine user home directory from environment" );
  }

  String configDir = configRoot == null ?
                     String::str( "%s/.config/" OZ_APPLICATION_NAME, home ) :
                     String::str( "%s/" OZ_APPLICATION_NAME, configRoot );

  String localDir = localRoot == null ?
                    String::str( "%s/.local/share/" OZ_APPLICATION_NAME, home ) :
                    String::str( "%s/" OZ_APPLICATION_NAME, localRoot );

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
  Log::printTime();
  Log::printEnd();

  OZ_MAIN_CALL( this, {
    if( SDL_Init( SDL_INIT_NOPARACHUTE | SDL_INIT_VIDEO | SDL_INIT_JOYSTICK ) != 0 ) {
      throw Exception( "Failed to initialise SDL: %s", SDL_GetError() );
    }
  } )
  initFlags |= INIT_SDL;

  PhysFile::init( File::TEMPORARY, 32*1024*1024 );
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

  File configFile( configDir + "/client.rc" );
  if( config.load( configFile ) ) {
    Log::printEnd( "Configuration read from '%s'", configFile.path().cstr() );

    if( String::equals( config.get( "_version", "" ), OZ_APPLICATION_VERSION ) ) {
      initFlags |= INIT_CONFIG;
    }
    else {
      Log::println( "Invalid configuration file version, configuration will be cleaned and written "
                    "upon exit" );

      config.add( "_version", OZ_APPLICATION_VERSION );
      config.get( "_version", "" );
    }
  }
  else {
    Log::println( "No configuration file, default configuration will be used and written upon "
                  "exit" );
    config.add( "_version", OZ_APPLICATION_VERSION );
    config.get( "_version", "" );
  }

  config.add( "dir.config", configDir );
  config.add( "dir.local", localDir );

  // tag variables as used
  config.get( "dir.config", "" );
  config.get( "dir.local", "" );

  String prefix = config.getSet( "dir.prefix", OZ_INSTALL_PREFIX );
  String dataDir( prefix + "/share/" OZ_APPLICATION_NAME );

#ifndef __native_client__

  // Don't mess with screensaver. In X11 it only makes effect for windowed mode, in fullscreen
  // mode screensaver never starts anyway. Turning off screensaver has a side effect: if the game
  // crashes, it remains turned off. Besides that, in X11 several programs (e.g. IM clients) rely
  // on screensaver's counter, so they don't detect that you are away if the screensaver is screwed.
  static char allowScreensaverEnv[] = "SDL_VIDEO_ALLOW_SCREENSAVER=1";
  SDL_putenv( allowScreensaverEnv );

  int  windowWidth      = config.getSet( "window.width", 0 );
  int  windowHeight     = config.getSet( "window.height", 0 );
  int  windowBpp        = config.getSet( "window.bpp", 0 );
  bool windowFullscreen = config.getSet( "window.fullscreen", true );
  bool enableVSync      = config.getSet( "window.vsync", true );

  uint windowFlags = SDL_OPENGL;

  if( windowFullscreen ) {
    windowFlags |= SDL_FULLSCREEN;
  }

  const SDL_VideoInfo* videoInfo = SDL_GetVideoInfo();

  Log::verboseMode = true;
  Log::println( "Desktop video mode: %dx%d-%d",
                videoInfo->current_w, videoInfo->current_h, videoInfo->vfmt->BitsPerPixel );
  Log::verboseMode = false;

  if( windowWidth == 0 || windowHeight == 0 ) {
    windowWidth  = videoInfo->current_w;
    windowHeight = videoInfo->current_h;
  }
  if( windowBpp == 0 ) {
    windowBpp = videoInfo->vfmt->BitsPerPixel;
  }

  SDL_GL_SetAttribute( SDL_GL_SWAP_CONTROL, enableVSync );

  Log::print( "Creating OpenGL window %dx%d-%d [%s] ...",
              windowWidth, windowHeight, windowBpp, windowFullscreen ? "fullscreen" : "windowed" );

  if( SDL_VideoModeOK( windowWidth, windowHeight, windowBpp, windowFlags ) == 1 ) {
    throw Exception( "Video mode not supported" );
  }

  SDL_Surface* window = SDL_SetVideoMode( windowWidth, windowHeight, windowBpp, windowFlags );

  if( window == null ) {
    throw Exception( "Window creation failed" );
  }

  SDL_WM_SetCaption( OZ_APPLICATION_TITLE " " OZ_APPLICATION_VERSION,
                     OZ_APPLICATION_TITLE " " OZ_APPLICATION_VERSION );

  windowWidth  = window->w;
  windowHeight = window->h;
  windowBpp    = window->format->BitsPerPixel;

  Log::printEnd( " %dx%d-%d ... OK", windowWidth, windowHeight, windowBpp );

  SDL_ShowCursor( SDL_FALSE );

#endif

  ui::keyboard.init();
  ui::mouse.init();

  Log::println( "Content search path {" );
  Log::indent();

#ifdef __native_client__

  // Packages must be hardcoded as dir listing is not supported on NaCl.
  File pkgs[] = {
    File( "/data/ozbase.7z" ),
    File( "/data/openzone.7z" )
  };

  bool createdPkgs = false;

  foreach( pkg, iter( pkgs ) ) {
    if( !pkg->stat() ) {
      pkg->write( pkg->path(), pkg->path().length() );

      Log::println( "Created empty file %s", pkg->path().cstr() );
      createdPkgs = true;
    }
  }
  if( createdPkgs ) {
    throw Exception( "Empty game data files created" );
  }

  foreach( pkg, iter( pkgs ) ) {
    if( PhysFile::mount( pkg->path(), null, true ) ) {
      Log::println( "%s", pkg->path().cstr() );
    }
    else {
      throw Exception( "Failed to mount '%s' on / in PhysicsFS", pkg->path().cstr() );
    }
  }

#else

  const char* userMusicPath = config.getSet( "dir.music", "" );

  if( !String::isEmpty( userMusicPath ) ) {
    if( !PhysFile::mount( userMusicPath, "/music", true ) ) {
      throw Exception( "Failed to mount '%s' on /music in PhysicsFS", userMusicPath );
    }
    Log::println( "%s [mounted on /music]", userMusicPath );
  }

  if( PhysFile::mount( localDir, null, true ) ) {
    Log::println( "%s", localDir.cstr() );

    DArray<File> list = File( localDir ).ls();

    foreach( file, list.citer() ) {
      if( file->hasExtension( "zip" ) || file->hasExtension( "7z" ) ) {
        if( !PhysFile::mount( file->path(), null, true ) ) {
          throw Exception( "Failed to mount '%s' on / in PhysicsFS", file->path().cstr() );
        }
        Log::println( "%s", file->path().cstr() );
      }
    }
  }

  if( PhysFile::mount( dataDir, null, true ) ) {
    Log::println( "%s", dataDir.cstr() );

    DArray<File> list = File( dataDir ).ls();

    foreach( file, list.citer() ) {
      if( file->hasExtension( "zip" ) || file->hasExtension( "7z" ) ) {
        if( !PhysFile::mount( file->path(), null, true ) ) {
          throw Exception( "Failed to mount '%s' on / in PhysicsFS", file->path().cstr() );
        }
        Log::println( "%s", file->path().cstr() );
      }
    }
  }

#endif

  Log::unindent();
  Log::println( "}" );

  if( String::equals( config.getSet( "seed", "TIME" ), "TIME" ) ) {
    int seed = int( Time::uclock() );
    Math::seed( seed );
    Log::println( "Random generator seed set to the current time: %u", seed );
  }
  else {
    int seed = config.get( "seed", 0 );
    Math::seed( seed );
    Log::println( "Random generator seed set to: %d", seed );
  }

  const char* locale = config.getSet( "lingua", "en" );

  Log::print( "Setting localisation '%s' ...", locale );
  if( lingua.init( locale ) ) {
    Log::printEnd( " OK" );
    initFlags |= INIT_LINGUA;
  }
  else {
    Log::printEnd( " Failed" );
  }

  initFlags |= INIT_LIBRARY;
  library.init();

  initFlags |= INIT_CONTEXT;
  context.init();

  initFlags |= INIT_RENDER;
#ifdef __native_client__
  OZ_MAIN_CALL( this, {
    render.init( null, System::width, System::height );
  } )
#else
  render.init( window, windowWidth, windowHeight );
#endif

  initFlags |= INIT_AUDIO;
  sound.init();

  initFlags |= INIT_STAGE_INIT;
  menuStage.init();
  gameStage.init();

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

  ui::mouse.reset();

  SDL_Event event;

  bool isAlive        = true;
  bool isActive       = true;
  bool hasMouseFocus  = true;

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
    if( isBenchmark && float( timer.micros / 1000000 ) >= benchmarkTime ) {
      isAlive = false;
    }

    // read input & events
    ui::keyboard.prepare();
    ui::mouse.prepare();

    while( SDL_PollEvent( &event ) != 0 ) {
      switch( event.type ) {
        case SDL_MOUSEBUTTONUP: {
          ui::mouse.currButtons &= char( ~SDL_BUTTON( event.button.button ) );
          break;
        }
        case SDL_MOUSEBUTTONDOWN: {
          ui::mouse.buttons     |= char( SDL_BUTTON( event.button.button ) );
          ui::mouse.currButtons |= char( SDL_BUTTON( event.button.button ) );

          if( ui::mouse.buttons & SDL_BUTTON( SDL_BUTTON_WHEELUP ) ) {
            ++ui::mouse.relW;
          }
          if( ui::mouse.buttons & SDL_BUTTON( SDL_BUTTON_WHEELDOWN ) ) {
            --ui::mouse.relW;
          }
          break;
        }
        case SDL_KEYDOWN: {
          const SDL_keysym& keysym = event.key.keysym;

          ui::keyboard.keys[keysym.sym] |= SDL_PRESSED;

#ifndef __native_client__
          if( keysym.sym == SDLK_F11 ) {
            if( keysym.mod == 0 ) {
              loader.makeScreenshot();
            }
            else if( keysym.mod & KMOD_ALT ) {
              if( SDL_WM_ToggleFullScreen( window ) != 0 ) {
                windowFullscreen = !windowFullscreen;

                ui::mouse.isJailed = true;
                ui::mouse.reset();

                SDL_ShowCursor( false );
              }
            }
            else if( keysym.mod & KMOD_CTRL ) {
              ui::mouse.isJailed = !ui::mouse.isJailed;

              SDL_ShowCursor( !ui::mouse.isJailed );
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
            hasMouseFocus = event.active.gain != 0;
          }
          if( event.active.state & SDL_APPACTIVE ) {
            if( event.active.gain ) {
              ui::mouse.reset();

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
          Log::println( "Terminal signal or quit event received, exiting ..." );
          isAlive = false;
          break;
        }
      }
    }

    ui::mouse.update( hasMouseFocus );

    // Waste time when iconified.
    if( !isActive ) {
      Time::usleep( Timer::TICK_MICROS );

      timeSpent = Time::uclock() - timeLast;
      timeLast += timeSpent;

      continue;
    }

    timer.tick();

    isAlive &= stage->update();

    if( Stage::nextStage != null || !isAlive ) {
      stage->unload();

      if( isAlive ) {
        stage = Stage::nextStage;
        Stage::nextStage = null;

        stage->load();

        timeLast = Time::uclock();
      }
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

  return EXIT_SUCCESS;
}

}
}
