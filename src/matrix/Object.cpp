/*
 *  Object.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "matrix/Object.hpp"

#include "matrix/Lua.hpp"
#include "matrix/Synapse.hpp"

namespace oz
{

  const float Object::MOMENTUM_INTENSITY_COEF = -0.1f;
  const float Object::DAMAGE_INTENSITY_COEF   = 0.02f;

  Pool<Object::Event> Object::Event::pool;
  Pool<Object> Object::pool;

  Object::~Object()
  {
    assert( dim.x <= AABB::REAL_MAX_DIM );
    assert( dim.y <= AABB::REAL_MAX_DIM );

    events.free();
  }

  void Object::onDestroy()
  {
    synapse.genParts( clazz->nDebris, p, Vec3::ZERO, clazz->debrisVelocitySpread,
                      clazz->debrisColour, clazz->debrisColourSpread,
                      clazz->debrisRejection, clazz->debrisMass, clazz->debrisLifeTime );

    if( !clazz->onDestroy.isEmpty() ) {
      lua.call( clazz->onDestroy, this );
    }
  }

  void Object::onDamage( float damage )
  {
    if( !clazz->onDamage.isEmpty() ) {
      lua.damage = damage;
      lua.call( clazz->onDamage, this );
    }
  }

  void Object::onHit( const Hit*, float hitMomentum )
  {
    if( !clazz->onHit.isEmpty() ) {
      lua.hitMomentum = hitMomentum;
      lua.call( clazz->onHit, this );
    }
  }

  void Object::onUse( Bot* user )
  {
    if( !clazz->onUse.isEmpty() ) {
      lua.call( clazz->onUse, this, user );
    }
  }

  void Object::onUpdate()
  {
    if( !clazz->onUpdate.isEmpty() ) {
      lua.call( clazz->onUpdate, this );
    }
  }

  void Object::onAct()
  {
    if( !clazz->onAct.isEmpty() ) {
      lua.call( clazz->onAct, this );
    }
  }

  void Object::readFull( InputStream* istream )
  {
    p        = istream->readPoint3();
    h        = istream->readFloat();
    flags    = istream->readInt();
    oldFlags = istream->readInt();
    life     = istream->readFloat();

    int nEvents = istream->readInt();
    for( int i = 0; i < nEvents; ++i ) {
      int id = istream->readInt();
      float intensity = istream->readFloat();

      addEvent( id, intensity );
    }
  }

  void Object::writeFull( OutputStream* ostream ) const
  {
    ostream->writePoint3( p );
    ostream->writeFloat( h );
    ostream->writeInt( flags );
    ostream->writeInt( oldFlags );
    ostream->writeFloat( life );

    ostream->writeInt( events.length() );
    foreach( event, events.citer() ) {
      ostream->writeInt( event->id );
      ostream->writeFloat( event->intensity );
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
  }

  void Object::writeUpdate( OutputStream* ostream ) const
  {
    ostream->writeFloat( life );

    ostream->writeInt( events.length() );
    foreach( event, events.citer() ) {
      ostream->writeInt( event->id );
      ostream->writeFloat( event->intensity );
    }
  }

}
