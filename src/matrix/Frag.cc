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
 * @file matrix/Frag.cc
 */

#include <matrix/Frag.hh>

namespace oz
{

const float Frag::DAMAGE_THRESHOLD = 50.0f;

Pool<Frag, 2048> Frag::mpool;

Frag::Frag( const FragPool* pool_, int index_, const Point& p_, const Vec3& velocity_ )
{
  cell       = nullptr;
  index      = index_;
  poolId     = pool_->id;
  pool       = pool_;
  p          = p_;
  velocity   = velocity_;
  life       = pool->life;
  mass       = pool->mass;
  elasticity = pool->elasticity;
}

Frag::Frag( const FragPool* pool_, InputStream* is )
{
  cell       = nullptr;
  index      = is->readInt();
  poolId     = pool_->id;
  pool       = pool_;
  p          = is->readPoint();
  velocity   = is->readVec3();
  life       = is->readFloat();
  mass       = pool->mass;
  elasticity = pool->elasticity;
}

Frag::Frag( const FragPool* pool_, const JSON& json )
{
  cell       = nullptr;
  index      = json["index"].asInt();
  poolId     = pool_->id;
  pool       = pool_;
  p          = json["p"].asPoint();
  velocity   = json["velocity"].asVec3();
  life       = json["life"].asFloat();
  mass       = pool->mass;
  elasticity = pool->elasticity;
}

void Frag::write( OutputStream* os ) const
{
  os->writeInt( index );
  os->writePoint( p );
  os->writeVec3( velocity );
  os->writeFloat( life );
}

JSON Frag::write() const
{
  JSON json( JSON::OBJECT );

  json.add( "pool", pool->name );

  json.add( "index", index );
  json.add( "p", p );
  json.add( "velocity", velocity );
  json.add( "life", life );

  return json;
}

void Frag::readUpdate( InputStream* )
{}

void Frag::writeUpdate( OutputStream* )
{}

}
