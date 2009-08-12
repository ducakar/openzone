/*
 *  DynObject.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#include "precompiled.h"

#include "DynObject.h"

namespace oz
{

  void DynObject::readFull( InputStream *istream )
  {
    Object::readFull( istream );

    velocity = istream->readVec3();
    momentum = istream->readVec3();
    lower    = istream->readInt();
    floor    = istream->readVec3();
  }

  void DynObject::writeFull( OutputStream *ostream )
  {
    Object::writeFull( ostream );

    ostream->writeVec3( velocity );
    ostream->writeVec3( momentum );
    ostream->writeInt( lower );
    ostream->writeVec3( floor );
  }

  void DynObject::readUpdate( InputStream *istream )
  {
    p        = istream->readVec3();
    life     = istream->readFloat();
    velocity = istream->readVec3();
    momentum = istream->readVec3();

    int nEvents = istream->readInt();
    for( int i = 0; i < nEvents; i++ ) {
      addEvent( istream->readInt() );
    }
  }

  void DynObject::writeUpdate( OutputStream *ostream )
  {
    ostream->writeVec3( p );
    ostream->writeFloat( life );
    ostream->writeVec3( velocity );
    ostream->writeVec3( momentum );

    ostream->writeInt( events.length() );
    foreach( event, events.iterator() ) {
      ostream->writeInt( event->id );
    }
  }

}
