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
 * @file client/openzone/openzone.cc
 */

#include "stable.hh"

#include "ozmain/main.hh"

#include "client/Client.hh"

using namespace oz;

int main( int argc, char** argv )
{
  System::init();

  int exitCode = EXIT_FAILURE;

  printf( "OpenZone  Copyright © 2002-2012 Davorin Učakar\n"
          "This program comes with ABSOLUTELY NO WARRANTY.\n"
          "This is free software, and you are welcome to redistribute it\n"
          "under certain conditions; See COPYING file for details.\n\n" );

  try {
    exitCode = client::client.main( argc, argv );
    client::client.shutdown();
  }
  catch( const std::exception& e ) {
    Log::verboseMode = false;
    Log::printException( &e );

    System::bell();
    System::abort();
  }

  Log::verboseMode = true;
  Alloc::printLeaks();
  Log::verboseMode = false;

  if( Alloc::count != 0 ) {
    Log::println( "There are some memory leaks. See '%s' for details.", Log::logFile() );
  }

  return exitCode;
}
