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

const float Terra::Quad::SIZE     = float( SIZEI );
const float Terra::Quad::INV_SIZE = 1.0f / SIZE;
const float Terra::Quad::DIM      = SIZE / 2.0f;

const float Terra::DIM            = QUADS * Terra::Quad::DIM;

Terra::Terra() : id( -1 )
{}

void Terra::load( int id_ )
{
  id = id_;

  const String& name = library.terrae[id].name;
  const String& path = library.terrae[id].path;

  log.print( "Loading terrain '%s' ...", name.cstr() );

  PhysFile file( path );
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
      quads[x][y].vertex       = is.readPoint3();
      quads[x][y].triNormal[0] = is.readVec3();
      quads[x][y].triNormal[1] = is.readVec3();
    }
  }

  file.unmap();

  log.printEnd( " OK" );
}

void Terra::init()
{
  for( int x = 0; x < VERTS; ++x ) {
    for( int y = 0; y < VERTS; ++y ) {
      quads[x][y].vertex.x = float( x * Quad::SIZEI ) - DIM;
      quads[x][y].vertex.y = float( y * Quad::SIZEI ) - DIM;
      quads[x][y].vertex.z = 0.0f;
      quads[x][y].triNormal[0] = Vec3::ZERO;
      quads[x][y].triNormal[1] = Vec3::ZERO;
    }
  }
}

void Terra::read( InputStream* istream )
{
  String name = istream->readString();
  int id = library.terraIndex( name );

  load( id );
}

void Terra::write( BufferStream* ostream ) const
{
  ostream->writeString( library.terrae[id].name );
}

}
}
