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
 * @file matrix/Object.cc
 */

#include "stable.hh"

#include "matrix/Object.hh"

#include "matrix/Lua.hh"
#include "matrix/Synapse.hh"
#include "matrix/Bot.hh"

namespace oz
{
namespace matrix
{

const float Object::MOMENTUM_DAMAGE_COEF    = 1.00f;
const float Object::MOMENTUM_INTENSITY_COEF = 0.10f;
const float Object::DAMAGE_BASE_INTENSITY   = 0.50f;
const float Object::DAMAGE_INTENSITY_COEF   = 0.05f;
const Vec3  Object::DESTRUCT_FRAG_VELOCITY  = Vec3( 0.0f, 0.0f, 2.0f );

Pool<Object::Event, 256> Object::Event::pool;
Pool<Object, 16384>      Object::pool;

void Object::onDestroy()
{
  hard_assert( cell != null );

  for( int i = 0; i < items.length(); ++i ) {
    Object* obj = orbis.objects[ items[i] ];

    if( obj != null ) {
      obj->destroy();
    }
  }

  if( !clazz->onDestroy.isEmpty() ) {
    lua.objectCall( clazz->onDestroy, this );
  }

  if( clazz->fragPool != null ) {
    synapse.genFrags( clazz->fragPool,
                      clazz->nFrags,
                      Bounds( Point3( p.x - dim.x, p.y - dim.y, p.z ),
                              Point3( p.x + dim.x, p.y + dim.y, p.z + dim.z ) ),
                      DESTRUCT_FRAG_VELOCITY );
  }
}

void Object::onDamage( float )
{
  hard_assert( !clazz->onDamage.isEmpty() );

  lua.objectCall( clazz->onDamage, this );
}

void Object::onHit( const Hit*, float )
{
  hard_assert( !clazz->onHit.isEmpty() );

  lua.objectCall( clazz->onHit, this );
}

bool Object::onUse( Bot* user )
{
  lua.objectCall( clazz->onUse, this, user );
  return !lua.hasUseFailed;
}

void Object::onUpdate()
{
  hard_assert( !clazz->onUpdate.isEmpty() );

  lua.objectCall( clazz->onUpdate, this );
}

Object::~Object()
{
  hard_assert( dim.x <= AABB::REAL_MAX_DIM );
  hard_assert( dim.y <= AABB::REAL_MAX_DIM );

  events.free();
}

Object::Object( const ObjectClass* clazz_, int index_, const Point3& p_, Heading heading )
{
  p          = p_;
  dim        = clazz_->dim;
  cell       = null;
  index      = index_;
  flags      = clazz_->flags | heading;
  life       = clazz_->life;
  resistance = clazz_->resistance;
  clazz      = clazz_;

  if( flags & WEST_EAST_MASK ) {
    swap( dim.x, dim.y );
  }

  if( clazz->nItems != 0 ) {
    items.alloc( clazz->nItems );
  }
}

Object::Object( const ObjectClass* clazz_, InputStream* istream )
{
  p          = istream->readPoint3();
  dim        = clazz_->dim;
  cell       = null;
  index      = istream->readInt();
  flags      = istream->readInt();
  life       = istream->readFloat();
  resistance = clazz_->resistance;
  clazz      = clazz_;

  if( flags & WEST_EAST_MASK ) {
    swap( dim.x, dim.y );
  }

  int nEvents = istream->readInt();
  for( int i = 0; i < nEvents; ++i ) {
    int   id        = istream->readInt();
    float intensity = istream->readFloat();

    addEvent( id, intensity );
  }

  if( clazz->nItems != 0 ) {
    items.alloc( clazz->nItems );

    int nItems = istream->readInt();
    for( int i = 0; i < nItems; ++i ) {
      items.add( istream->readInt() );
    }
  }
}

void Object::write( BufferStream* ostream ) const
{
  ostream->writePoint3( p );
  ostream->writeInt( index );
  ostream->writeInt( flags );
  ostream->writeFloat( life );

  ostream->writeInt( events.length() );
  foreach( event, events.citer() ) {
    ostream->writeInt( event->id );
    ostream->writeFloat( event->intensity );
  }

  if( clazz->nItems != 0 ) {
    ostream->writeInt( items.length() );
    foreach( item, items.citer() ) {
      ostream->writeInt( *item );
    }
  }
}

void Object::readUpdate( InputStream* )
{}

void Object::writeUpdate( BufferStream* ) const
{}

}
}
