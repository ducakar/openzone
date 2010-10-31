/*
 *  openzone.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "client/Main.hpp"

#include <SDL_main.h>

using namespace oz;

int main( int argc, char** argv )
{
  printf( "OpenZone  Copyright (C) 2002-2009  Davorin Učakar\n"
      "This program comes with ABSOLUTELY NO WARRANTY.\n"
      "This is free software, and you are welcome to redistribute it\n"
      "under certain conditions; See COPYING file for details.\n\n" );

  try {
    client::main.main( &argc, argv );
  }
  catch( const Exception& e ) {
    oz::log.resetIndent();
    oz::log.println();
    oz::log.printRaw( "EXCEPTION: %s:%d: %s: %s", e.file, e.line, e.function, e.what() );
    oz::log.println();

    if( oz::log.isFile() ) {
      fprintf( stderr, "\nEXCEPTION: %s:%d: %s: %s\n", e.file, e.line, e.function, e.what() );
    }
  }
  catch( const std::exception& e ) {
    oz::log.resetIndent();
    oz::log.println();
    oz::log.printRaw( "EXCEPTION: %s", e.what() );
    oz::log.println();

    if( oz::log.isFile() ) {
      fprintf( stderr, "EXCEPTION: %s\n", e.what() );
    }
  }
  client::main.shutdown();

  return 0;
}
