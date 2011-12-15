/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
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
 */

/**
 * @file client/openzone/openzone.cc
 */

#include "stable.hh"

#include "client/Client.hh"

#include <SDL/SDL_main.h>

bool oz::Alloc::isLocked = true;

int main( int argc, char** argv )
{
  oz::System::init();
  oz::Alloc::isLocked = false;

  int exitCode = EXIT_FAILURE;

  printf( "OpenZone  Copyright (C) 2002-2011  Davorin Učakar\n"
      "This program comes with ABSOLUTELY NO WARRANTY.\n"
      "This is free software, and you are welcome to redistribute it\n"
      "under certain conditions; See COPYING file for details.\n\n" );

  try {
    exitCode = oz::client::client.main( argc, argv );
    oz::client::client.shutdown();
  }
  catch( const std::exception& e ) {
    oz::log.setError( true );
    oz::log.printException( e );
    oz::log.setError( false );

    oz::System::bell();
    oz::System::abort( false );
  }

//   oz::Alloc::isLocked = true;
//   oz::Alloc::printLeaks();
  return exitCode;
}
