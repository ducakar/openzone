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
 * @file matrix/Terra.cc
 */

#include "stable.hh"

#include "matrix/Terra.hh"

#include "matrix/Library.hh"

namespace oz
{
namespace matrix
{

void Terra::reset()
{
  load( -1 );
}

void Terra::load( int id_ )
{
  id = id_;

  if( id == -1 ) {
    for( int x = 0; x < VERTS; ++x ) {
      for( int y = 0; y < VERTS; ++y ) {
        quads[x][y].vertex.z     = 0.0f;
        quads[x][y].triNormal[0] = Vec3( 0.0f, 0.0f, 1.0f );
        quads[x][y].triNormal[1] = Vec3( 0.0f, 0.0f, 1.0f );
      }
    }
  }
  else {
    const String& name = library.terrae[id].name;
    const String& path = library.terrae[id].path;

    Log::print( "Loading terrain '%s' ...", name.cstr() );

    PFile file( path );
    if( !file.map() ) {
      throw Exception( "Cannot map terra file" );
    }

    InputStream is = file.inputStream();

    int max = is.readInt();
    if( max != VERTS ) {
      throw Exception( "Invalid dimension %d, should be %d", max, VERTS );
    }

    for( int x = 0; x < VERTS; ++x ) {
      for( int y = 0; y < VERTS; ++y ) {
        quads[x][y].vertex.z = is.readFloat();
      }
    }

    for( int x = 0; x < QUADS; ++x ) {
      for( int y = 0; y < QUADS; ++y ) {
        if( x != QUADS && y != QUADS ) {
          const Point& a = quads[x    ][y    ].vertex;
          const Point& b = quads[x + 1][y    ].vertex;
          const Point& c = quads[x + 1][y + 1].vertex;
          const Point& d = quads[x    ][y + 1].vertex;

          quads[x][y].triNormal[0] = ~( ( c - b ) ^ ( a - b ) );
          quads[x][y].triNormal[1] = ~( ( a - d ) ^ ( c - d ) );
        }
      }
    }

    liquid = is.readInt();

    file.unmap();

    Log::printEnd( " OK" );
  }
}

void Terra::init()
{
  for( int x = 0; x < VERTS; ++x ) {
    for( int y = 0; y < VERTS; ++y ) {
      quads[x][y].vertex.x     = float( x * Quad::SIZE - DIM );
      quads[x][y].vertex.y     = float( y * Quad::SIZE - DIM );
    }
  }
}

void Terra::read( InputStream* istream )
{
  const char* name = istream->readString();
  int id = String::isEmpty( name ) ? -1 : library.terraIndex( name );

  load( id );
}

void Terra::write( BufferStream* ostream ) const
{
  const char* name = id == -1 ? "" : library.terrae[id].name.cstr();

  ostream->writeString( name );
}

}
}
