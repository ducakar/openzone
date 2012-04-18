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
 * @file matrix/Dynamic.cc
 */

#include "stable.hh"

#include "matrix/Dynamic.hh"

#include "matrix/Lua.hh"
#include "matrix/Synapse.hh"

namespace oz
{
namespace matrix
{

Pool<Dynamic, 4096> Dynamic::pool;

void Dynamic::onDestroy()
{
  if( !clazz->onDestroy.isEmpty() ) {
    lua.objectCall( clazz->onDestroy, this );
  }

  if( clazz->fragPool != null ) {
    if( cell == null ) {
      hard_assert( parent != -1 );

      const Object* container = orbis.objects[parent];
      if( container == null ) {
        return;
      }
      else {
        p = container->p;
      }
    }

    synapse.gen( clazz->fragPool,
                 clazz->nFrags,
                 Bounds( Point( p.x - dim.x, p.y - dim.y, p.z ),
                         Point( p.x + dim.x, p.y + dim.y, p.z + dim.z ) ),
                 velocity + DESTRUCT_FRAG_VELOCITY );
  }
}

Dynamic::Dynamic( const DynamicClass* clazz_, int index_, const Point& p_, Heading heading ) :
  Object( clazz_, index_, p_, heading )
{
  velocity = Vec3::ZERO;
  momentum = Vec3::ZERO;
  floor    = Vec3( 0.0f, 0.0f, 1.0f );
  parent   = -1;
  lower    = -1;
  depth    = 0.0f;
  mass     = clazz_->mass;
  lift     = clazz_->lift;
}

Dynamic::Dynamic( const DynamicClass* clazz_, InputStream* istream ) :
  Object( clazz_, istream )
{
  velocity = istream->readVec3();
  momentum = istream->readVec3();
  floor    = istream->readVec3();
  parent   = istream->readInt();
  lower    = istream->readInt();
  depth    = istream->readFloat();
  mass     = clazz_->mass;
  lift     = clazz_->lift;
}

void Dynamic::write( BufferStream* ostream ) const
{
  Object::write( ostream );

  ostream->writeVec3( velocity );
  ostream->writeVec3( momentum );
  ostream->writeVec3( floor );
  ostream->writeInt( parent );
  ostream->writeInt( lower );
  ostream->writeFloat( depth );
}

void Dynamic::readUpdate( InputStream* )
{}

void Dynamic::writeUpdate( BufferStream* ) const
{}

}
}
