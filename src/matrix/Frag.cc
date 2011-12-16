/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2011 Davorin Učakar
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
 * @file matrix/Frag.cc
 */

#include "stable.hh"

#include "matrix/Frag.hh"

#include "matrix/FragPool.hh"

namespace oz
{
namespace matrix
{

const float Frag::DAMAGE_THRESHOLD = 50.0f;

Pool<Frag, 2048> Frag::mpool;

Frag::Frag( const FragPool* pool_, int index_, const Point3& p_, const Vec3& velocity_ )
{
  cell        = null;
  index       = index_;
  pool        = pool_;
  p           = p_;
  velocity    = velocity_;
  life        = pool->life;
  mass        = pool->mass;
  restitution = pool->restitution;
}

Frag::Frag( const FragPool* pool_, InputStream* istream )
{
  cell        = null;
  index       = istream->readInt();
  pool        = pool_;
  p           = istream->readPoint3();
  velocity    = istream->readVec3();
  life        = istream->readFloat();
  mass        = pool->mass;
  restitution = pool->restitution;
}

void Frag::write( BufferStream* ostream )
{
  ostream->writeInt( index );
  ostream->writePoint3( p );
  ostream->writeVec3( velocity );
  ostream->writeFloat( life );
}

void Frag::readUpdate( InputStream* )
{}

void Frag::writeUpdate( BufferStream* )
{}

}
}
