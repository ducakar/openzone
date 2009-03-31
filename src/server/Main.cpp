/*
 *  Main.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#include "precompiled.h"

#include "Main.h"

#ifndef WIN32
#include <unistd.h>
#include <sys/stat.h>
#endif

namespace oz
{
namespace server
{

  Main main;

  void Main::defaultConfig()
  {
    config.add( "data",                               "/usr/share/dark/data" );
    config.add( "tick",                               "20" );
  }

  void Main::shutdown()
  {
    if( initFlags & INIT_SDL ) {
      logFile.print( "Shutting down SDL ..." );
      SDL_Quit();
      logFile.printEnd( " OK" );
    }
    logFile.printlnETD( "%s finished on", OZ_APP_NAME );
  }

  void Main::main()
  {
    const char *homeVar = getenv( "HOME" );
    String home( homeVar == null ? "./" OZ_RC_DIR : homeVar + String( "/" OZ_RC_DIR ) );

#ifdef WIN32
#else
    struct stat homeDirStat;
    if( stat( home.cstr(), &homeDirStat ) ) {
      printf( "No resource dir found, creating '%s' ...", home.cstr() );

      if( mkdir( home.cstr(), S_IRUSR | S_IWUSR | S_IXUSR ) ) {
        printf( " Failed\n" );
        shutdown();
        return;
      }
      printf( " OK\n" );
    }
#endif

#ifdef OZ_LOG_FILE
    String logPath = home + OZ_LOG_FILE;

    if( !logFile.init( logPath, true, "  " ) ) {
      printf( "Can't create/open log file '%s' for writing\n", logPath.cstr() );
      shutdown();
      return;
    }
    logFile.println( "Log file '%s'", logPath.cstr() );
    printf( "Log file '%s'\n", logPath.cstr() );
#else
    logFile.init( null, true, "  " );
    logFile.println( "Log stream stdout ... OK" );
#endif

    logFile.printlnETD( "%s started on", OZ_APP_NAME );

    logFile.print( "Initializing SDL ..." );
    if( SDL_Init( 0 ) ) {
      logFile.printEnd( " Failed" );
      shutdown();
      return;
    }
    logFile.printEnd( " OK" );

    initFlags |= INIT_SDL;

    logFile.print( "Loading default config ..." );
    defaultConfig();
    logFile.printEnd( " OK" );

    const char *configPath = ( home + OZ_CONFIG_FILE ).cstr();

    if( !config.load( configPath ) ) {
      logFile.println( "Config not found, creating default {", configPath );
      logFile.indent();

      printf( "Config not found, creating default '%s' ...", configPath );

      if( !config.save( configPath ) ) {
        printf( " Failed\n" );
        shutdown();
        return;
      }
      printf( " OK\n" );

      logFile.unindent();
      logFile.println( "}" );
    }

    const char *data = config.get( "data", "/usr/share/openzone" );

    logFile.print( "Going to working directory '%s' ...", (const char*) data );

#ifdef WIN32
#else
    if( chdir( data ) != 0 ) {
      logFile.printEnd(" Failed");
      shutdown();
      return;
    }
    else {
      logFile.printEnd(" OK");
    }
#endif

    logFile.println( "MAIN LOOP {" );
    logFile.indent();

    bool isAlive = true;

    Uint32 tick     = config.get( "tick", 20 );
    Uint32 time;
    Uint32 timeZero = SDL_GetTicks();
    Uint32 timeLast = timeZero;

    // THE MAGNIFICAT MAIN LOOP
    do {
      // update world
//       isAlive &= client.update( tick );

      // if there's still some time left, waste it
      time = SDL_GetTicks() - timeLast;

      if( time < tick ) {
        SDL_Delay( max( tick - time, 1u ) );
      }
      timeLast += tick;
    }
    while( isAlive );

    logFile.unindent();
    logFile.println( "}" );

    shutdown();
  }

}
}

int main( int, char *[] )
{
  oz::server::main.main();
  return 0;
}
