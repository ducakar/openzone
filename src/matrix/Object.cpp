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
    assert( dim.x <= AABB::REAL_MAX_DIMXY );
    assert( dim.y <= AABB::REAL_MAX_DIMXY );

    events.free();
  }

  void Object::onUpdate()
  {}

  void Object::onHit( const Hit*, float )
  {}

  void Object::onDestroy()
  {}

  void Object::onUse( Object* )
  {}

  void Object::onPut( Object* )
  {}

  void Object::onCut( Object* )
  {}

  void Object::readFull( InputStream *istream )
  {
    p        = istream->readVec3();
    flags    = istream->readInt();
    oldFlags = istream->readInt();
    damage   = istream->readFloat();
  }

  void Object::writeFull( OutputStream *ostream )
  {
    ostream->writeVec3( p );
    ostream->writeInt( flags );
    ostream->writeInt( oldFlags );
    ostream->writeFloat( damage );
  }

  void Object::readUpdate( InputStream* )
  {}

  void Object::writeUpdate( OutputStream* )
  {}

}
