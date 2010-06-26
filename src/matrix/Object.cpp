/*
 *  Object.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING file for details.
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
    synapse.genParts( type->nDebris, p, Vec3::zero(), type->debrisVelocitySpread,
                      type->debrisColor, type->debrisColorSpread,
                      type->debrisRejection, type->debrisMass, type->debrisLifeTime );

    if( !type->onDestroy.isEmpty() ) {
      lua.call( type->onDestroy, this );
    }
  }

  void Object::onDamage( float damage )
  {
    if( !type->onDamage.isEmpty() ) {
      lua.damage = damage;
      lua.call( type->onDamage, this );
    }
  }

  void Object::onHit( const Hit*, float hitMomentum )
  {
    if( !type->onHit.isEmpty() ) {
      lua.hitMomentum = hitMomentum;
      lua.call( type->onHit, this );
    }
  }

  void Object::onUse( Bot* user )
  {
    if( !type->onUse.isEmpty() ) {
      lua.call( type->onUse, this, user );
    }
  }

  void Object::onUpdate()
  {
    if( !type->onUpdate.isEmpty() ) {
      lua.call( type->onUpdate, this );
    }
  }

  void Object::onAct()
  {
    if( !type->onAct.isEmpty() ) {
      lua.call( type->onAct, this );
    }
  }

  void Object::readFull( InputStream* istream )
  {
    p        = istream->readVec3();
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
    ostream->writeVec3( p );
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
