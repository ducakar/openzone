/*
 *  Main.cpp
 *
 *  Server initialization and main loop
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#include "precompiled.h"

#include "Main.h"

#include "Game.h"

#ifdef WIN32
# include <direct.h>
# include <sys/types.h>
# include <sys/stat.h>
#else
# include <unistd.h>
# include <sys/stat.h>
#endif

#include <SDL_net.h>

namespace oz
{
namespace server
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
    if( SDL_Init( 0 ) || SDLNet_Init() ) {
      logFile.printEnd( " Failed" );
      return;
    }
    logFile.printEnd( " OK" );

    initFlags |= INIT_SDL;

    String configPath = home + OZ_CONFIG_FILE;
    config.load( configPath );

    const char *data = config.get( "data", "/usr/share/openzone" );

    logFile.print( "Going to working directory '%s' ...", data );

    if( chdir( data ) != 0 ) {
      logFile.printEnd( " Failed" );
      return;
    }
    else {
      logFile.printEnd( " OK" );
    }

    logFile.println( "Initializing Game {" );
    logFile.indent();
    if( !game.init() ) {
      return;
    }
    logFile.unindent();
    logFile.println( "}" );
    initFlags |= INIT_GAME_INIT;

    logFile.println( "Starting Game {" );
    logFile.indent();
    game.start();
    logFile.unindent();
    logFile.println( "}" );
    initFlags |= INIT_GAME_START;

    logFile.println( "MAIN LOOP {" );
    logFile.indent();

    logFile.println( "MAIN LOOP {" );
    logFile.indent();

    uint tick     = config.get( "tick", 20 );
    // time passed form start of the frame
    uint time;
    uint timeZero = SDL_GetTicks();
    // time at start of the frame
    uint timeLast = timeZero;

    // THE MAGNIFICANT MAIN LOOP
    do {
      // update world
      game.update( tick );

      // render graphics, if we have enough time left
      time = SDL_GetTicks() - timeLast;

      if( time < tick ) {
        SDL_Delay( max( tick - time, 1u ) );
      }
      else if( time > 10 * tick ) {
        timeLast += time - tick;
      }
      timeLast += tick;
    }
    while( true );

    logFile.unindent();
    logFile.println( "}" );

    logFile.println( "Printing config at exit {" );
    logFile.print( "%s", config.toString( "  " ).cstr() );
    logFile.println( "}" );
  }

}
}

int main( int, char*[] )
{
  try {
    oz::server::main.main();
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
  oz::server::main.shutdown();

  return 0;
}
