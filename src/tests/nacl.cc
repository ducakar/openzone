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
 * @file tests/nacl.cc
 */

#include "oz/oz.hh"
#include "oz/windefs.h"

#include <SDL/SDL.h>
#include <GLES2/gl2.h>

using namespace oz;

static Barrier main;
static Barrier aux;

int ozMain( int, char** )
{
  System::init();

  Time t = Time::local();
  log.println( "[%02d:%02d:%02d] START", t.hour, t.minute, t.second );

  main.init();
  aux.init();

  File::init( File::TEMPORARY, 1024 );

  File file( "/drek.text" );

  if( file.stat() ) {
    log.println( "Type: %s", file.type() == File::DIRECTORY ? "DIRECTORY" : "REGULAR" );
    log.println( "Size: %d", file.size() );
  }

  Buffer b = file.read();

  putchar( '\"' );
  for( int i = 0; i < b.length(); ++i ) {
    putchar( b[i] );
  }
  puts( "\"" );

  File::free();

  aux.free();
  main.free();

  t = Time::local();
  log.println( "[%02d:%02d:%02d] END", t.hour, t.minute, t.second );
  return 0;
}
