/*
 * OpenZone - Simple Cross-Platform FPS/RTS Game Engine
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
 *
 * Davorin Učakar <davorin.ucakar@gmail.com>
 */

/**
 * @file matrix/Object.cpp
 */

#include "stable.hpp"

#include "matrix/Object.hpp"

#include "matrix/Lua.hpp"
#include "matrix/Synapse.hpp"
#include "matrix/Bot.hpp"

namespace oz
{
namespace matrix
{

const float Object::MOMENTUM_INTENSITY_COEF = -0.10f;
const float Object::DAMAGE_INTENSITY_COEF   = +0.02f;
const float Object::DAMAGE_BASE_INTENSITY   = +0.30f;

Pool<Object::Event, 256> Object::Event::pool;
Pool<Object, 16384>      Object::pool;

Object::~Object()
{
  hard_assert( dim.x <= AABB::REAL_MAX_DIM );
  hard_assert( dim.y <= AABB::REAL_MAX_DIM );

  events.free();
}

void Object::onDestroy()
{
  synapse.genParts( clazz->nDebris, p, Vec3::ZERO, clazz->debrisVelocitySpread,
                    clazz->debrisColour, clazz->debrisColourSpread,
                    clazz->debrisRejection, clazz->debrisMass, clazz->debrisLifeTime );

  for( int i = 0; i < items.length(); ++i ) {
    Object* obj = orbis.objects[ items[i] ];

    if( obj != null ) {
      obj->destroy();
    }
  }

  if( !clazz->onDestroy.isEmpty() ) {
    lua.objectCall( clazz->onDestroy, this );
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

void Object::readFull( InputStream* istream )
{
  p     = istream->readPoint3();
  flags = istream->readInt();
  life  = istream->readFloat();

  int nEvents = istream->readInt();
  for( int i = 0; i < nEvents; ++i ) {
    int id = istream->readInt();
    float intensity = istream->readFloat();

    addEvent( id, intensity );
  }

  int nItems = istream->readInt();
  for( int i = 0; i < nItems; ++i ) {
    int index = istream->readInt();

    items.add( index );
  }
}

void Object::writeFull( OutputStream* ostream ) const
{
  ostream->writePoint3( p );
  ostream->writeInt( flags );
  ostream->writeFloat( life );

  ostream->writeInt( events.length() );
  foreach( event, events.citer() ) {
    ostream->writeInt( event->id );
    ostream->writeFloat( event->intensity );
  }

  ostream->writeInt( items.length() );
  foreach( item, items.citer() ) {
    ostream->writeInt( *item );
  }
}

void Object::readUpdate( InputStream* istream )
{
  life = istream->readFloat();

  int nEvents = istream->readInt();
  for( int i = 0; i < nEvents; ++i ) {
    int   id        = istream->readInt();
    float intensity = istream->readFloat();

    addEvent( id, intensity );
  }

  int nItems = istream->readInt();
  for( int i = 0; i < nItems; ++i ) {
    int index = istream->readInt();

    items.add( index );
  }
}

void Object::writeUpdate( OutputStream* ostream ) const
{
  ostream->writeFloat( life );

  ostream->writeInt( events.length() );
  foreach( event, events.citer() ) {
    ostream->writeInt( event->id );
    ostream->writeFloat( event->intensity );
  }

  ostream->writeInt( items.length() );
  foreach( item, items.citer() ) {
    ostream->writeInt( *item );
  }
}

}
}
