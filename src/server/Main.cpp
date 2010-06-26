/*
 *  Main.cpp
 *
 *  Server initialisation and main loop
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING file for details.
 */

#include "stable.hpp"

#include "Main.hpp"

#include "Game.hpp"

#ifdef OZ_MINGW
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
    log.println( "Shutdown {" );
    log.indent();

    if( initFlags & INIT_GAME_START ) {
      log.println( "Stopping Game {" );
      log.indent();
      game.stop();
      log.unindent();
      log.println( "}" );
    }
    if( initFlags & INIT_GAME_INIT ) {
      log.println( "Shutting down Game {" );
      log.indent();
      game.free();
      log.unindent();
      log.println( "}" );
    }
    if( initFlags & INIT_SDL ) {
      log.print( "Shutting down SDL ..." );
      SDL_ShowCursor( true );
      SDLNet_Quit();
      SDL_Quit();
      log.printEnd( " OK" );
    }

    log.unindent();
    log.println( "}" );
    log.printlnETD( OZ_APP_NAME " finished at" );
  }

  void Main::main()
  {
    const char* homeVar = getenv( "HOME" );
    String home = String( homeVar == null ? OZ_RC_DIR "/" : homeVar + String( "/" OZ_RC_DIR "/" ) );

    class stat homeDirStat;
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

    log.print( "Initialising SDL ..." );
    if( SDL_Init( 0 ) || SDLNet_Init() ) {
      log.printEnd( " Failed" );
      return;
    }
    log.printEnd( " OK" );

    initFlags |= INIT_SDL;

    String configPath = home + OZ_CONFIG_FILE;
    config.load( configPath );

    const char* data = config.getSet( "data", "/usr/share/openzone" );

    log.print( "Going to working directory '%s' ...", data );

    if( chdir( data ) != 0 ) {
      log.printEnd( " Failed" );
      return;
    }
    else {
      log.printEnd( " OK" );
    }

    log.println( "Initialising Game {" );
    log.indent();
    if( !game.init() ) {
      return;
    }
    log.unindent();
    log.println( "}" );
    initFlags |= INIT_GAME_INIT;

    log.println( "Starting Game {" );
    log.indent();
    game.start();
    log.unindent();
    log.println( "}" );
    initFlags |= INIT_GAME_START;

    log.println( "MAIN LOOP {" );
    log.indent();

    log.println( "MAIN LOOP {" );
    log.indent();

    uint tick     = config.getSet( "tick", 20 );
    // time passed form start of the frame
    uint time;
    uint timeZero = SDL_GetTicks();
    // time at start of the frame
    uint timeLast = timeZero;

    // THE MAGNIFICANT MAIN LOOP
    do {
      // update world
      game.update();

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

    log.unindent();
    log.println( "}" );

    log.println( "Printing config at exit {" );
    log.print( "%s", config.toString( "  " ).cstr() );
    log.println( "}" );
  }

}
}

OZ_IMPORT()

int main( int, char** )
{
  try {
    server::main.main();
  }
  catch( Exception e ) {
    log.resetIndent();
    log.println();
    log.println( "*** EXCEPTION: %s line %d", e.file, e.line );
    log.println( "*** MESSAGE: %s", e.message );
    log.println();

    if( log.isFile() ) {
      printf( "*** EXCEPTION: %s line %d\n*** MESSAGE: %s\n\n", e.file, e.line, e.message );
    }
  }
  server::main.shutdown();

  return 0;
}
