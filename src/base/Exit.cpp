/*
 *  Exit.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#include "base.h"

#include <stdio.h>
#include <stdlib.h>

namespace oz
{

  Exit exit;

  void Exit::add( void ( *callback )() )
  {
    atexit( callback );
  }

  void Exit::commit( const char *message )
  {
    if( logFile.isFile() ) {
      printf( "\n*** %s\n*** Terminating ***", message );
    }
    logFile.printRaw( "\n*** %s\n*** Terminating ***", message );

    ::exit( EXIT_FAILURE );
  }

}
