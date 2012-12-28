/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2013 Davorin Učakar
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
 * @file matrix/FragPool.cc
 */

#include <stable.hh>
#include <matrix/FragPool.hh>

#include <matrix/Frag.hh>
#include <matrix/Liber.hh>

namespace oz
{
namespace matrix
{

FragPool::FragPool( InputStream* is, const char* name_, int id_ ) :
  name( name_ ), id( id_ )
{
  flags          = is->readInt();

  velocitySpread = is->readFloat();

  life           = is->readFloat();
  lifeSpread     = is->readFloat();

  mass           = is->readFloat();
  elasticity     = is->readFloat();

  int nModels    = is->readInt();
  models.allocate( nModels );

  for( int i = 0; i < nModels; ++i ) {
    const char* sModel = is->readString();

    models.add( liber.modelIndex( sModel ) );
  }
}

Frag* FragPool::create( int index, const Point& pos, const Vec3& velocity ) const
{
  return new Frag( this, index, pos, velocity );
}

Frag* FragPool::create( InputStream* istream ) const
{
  return new Frag( this, istream );
}

}
}
