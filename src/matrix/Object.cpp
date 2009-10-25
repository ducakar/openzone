/*
 *  Object.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#include "precompiled.h"

#include "Object.h"

namespace oz
{

  Object::~Object()
  {
    assert( dim.x <= AABB::REAL_MAX_DIM );
    assert( dim.y <= AABB::REAL_MAX_DIM );

    events.free();
  }

  void Object::onUpdate()
  {}

  void Object::onHit( const Hit*, float )
  {}

  void Object::onUse( Bot* )
  {}

  void Object::readFull( InputStream *istream )
  {
    p        = istream->readVec3();
    flags    = istream->readInt();
    oldFlags = istream->readInt();
    life     = istream->readFloat();

    int nEvents = istream->readInt();
    for( int i = 0; i < nEvents; i++ ) {
      int id = istream->readInt();
      float intensity = istream->readFloat();

      addEvent( id, intensity );
    }
  }

  void Object::writeFull( OutputStream *ostream ) const
  {
    ostream->writeVec3( p );
    ostream->writeInt( flags );
    ostream->writeInt( oldFlags );
    ostream->writeFloat( life );

    ostream->writeInt( events.length() );
    foreach( event, events.iterator() ) {
      ostream->writeInt( event->id );
      ostream->writeFloat( event->intensity );
    }
  }

  void Object::readUpdate( InputStream *istream )
  {
    life = istream->readFloat();

    int nEvents = istream->readInt();
    for( int i = 0; i < nEvents; i++ ) {
      int   id        = istream->readInt();
      float intensity = istream->readFloat();

      addEvent( id, intensity );
    }
  }

  void Object::writeUpdate( OutputStream *ostream ) const
  {
    ostream->writeFloat( life );

    ostream->writeInt( events.length() );
    foreach( event, events.iterator() ) {
      ostream->writeInt( event->id );
      ostream->writeFloat( event->intensity );
    }
  }

}
