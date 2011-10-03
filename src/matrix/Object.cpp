/*
 *  Object.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "matrix/Object.hpp"

#include "matrix/Lua.hpp"
#include "matrix/Synapse.hpp"
#include "matrix/Bot.hpp"

namespace oz
{

  const float Object::MOMENTUM_INTENSITY_COEF = -0.10f;
  const float Object::DAMAGE_INTENSITY_COEF   = +0.02f;

  Pool<Object::Event, 4096> Object::Event::pool;
  Pool<Object, 2048>        Object::pool;

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

  void Object::onUse( Bot* user )
  {
    lua.objectCall( clazz->onUse, this, user );
  }

  void Object::onUpdate()
  {
    hard_assert( !clazz->onUpdate.isEmpty() );

    lua.objectCall( clazz->onUpdate, this );
  }

  void Object::readFull( InputStream* istream )
  {
    p        = istream->readPoint3();
    flags    = istream->readInt();
    oldFlags = istream->readInt();
    life     = istream->readFloat();

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
    ostream->writeInt( oldFlags );
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
