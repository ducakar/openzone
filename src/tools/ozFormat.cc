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
 * @file tools/ozBuild.cc
 *
 * Data builder.
 */

#include "stable.hh"

#include "matrix/Library.hh"

#include "tools/ClassFormatter.hh"

#include <SDL/SDL_main.h>

bool oz::Alloc::isLocked = true;

namespace oz
{

static void shutdown()
{
  library.free();
  config.clear( true );
}

int main( int argc, char** argv )
{
  if( argc >= 2 ) {
    config.add( "dir.prefix", argv[1] );
  }
  else {
    log.println( "Usage: ozFormat <prefix_dir>" );
    return EXIT_FAILURE;
  }

  String prefixDir = config.get( "dir.prefix", OZ_INSTALL_PREFIX );
  String dataDir   = prefixDir + "/share/" OZ_APPLICATION_NAME;

  log.print( "Setting working directory to data directory '%s' ...", dataDir.cstr() );
  if( !File::chdir( dataDir ) ) {
    return EXIT_FAILURE;
  }
  log.printEnd( " OK" );

  library.buildInit();

  String srcDir = "class";
  File dir( srcDir );
  DArray<File> dirList = dir.ls();

  srcDir = srcDir + "/";

  foreach( file, dirList.citer() ) {
    if( !file->hasExtension( "rc" ) ) {
      continue;
    }

    classFormatter.format( file->baseName() );
  }

  return EXIT_SUCCESS;
}

}

int main( int argc, char** argv )
{
  oz::System::init( oz::System::CATCH_SIGNALS_BIT );
#ifndef NDEBUG
  oz::System::init( oz::System::CATCH_SIGNALS_BIT | oz::System::HALT_BIT );
#endif

  oz::Alloc::isLocked = false;

  int exitCode = EXIT_FAILURE;

  printf( "OpenZone  Copyright (C) 2002-2011  Davorin Učakar\n"
      "This program comes with ABSOLUTELY NO WARRANTY.\n"
      "This is free software, and you are welcome to redistribute it\n"
      "under certain conditions; See COPYING file for details.\n\n" );

  try {
    exitCode = oz::main( argc, argv );
  }
  catch( const oz::Exception& e ) {
    oz::log.resetIndent();
    oz::log.println();
    oz::log.printException( e );
    oz::log.println();

    if( oz::log.isFile() ) {
      fprintf( stderr, "\nEXCEPTION: %s\n", e.what() );
      fprintf( stderr, "  in %s\n\n", e.function );
      fprintf( stderr, "  at %s:%d\n\n", e.file, e.line );
    }

    return EXIT_FAILURE;
  }
  catch( const std::exception& e ) {
    oz::log.resetIndent();
    oz::log.println();
    oz::log.println();
    oz::log.println( "EXCEPTION: %s", e.what() );
    oz::log.println();

    if( oz::log.isFile() ) {
      fprintf( stderr, "\nEXCEPTION: %s\n\n", e.what() );
    }

    return EXIT_FAILURE;
  }

  oz::shutdown();

  oz::Alloc::isLocked = true;
  oz::Alloc::printLeaks();
  return exitCode;
}
