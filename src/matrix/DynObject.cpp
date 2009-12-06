/*
 *  DynObject.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3.0. See COPYING for details.
 */

#include "precompiled.h"

#include "DynObject.h"

#include "Synapse.h"

namespace oz
{

  void DynObject::readFull( InputStream *istream )
  {
    Object::readFull( istream );

    parent     = istream->readInt();
    velocity   = istream->readVec3();
    momentum   = istream->readVec3();
    floor      = istream->readVec3();
    lower      = istream->readInt();
    depth      = istream->readFloat();
  }

  void DynObject::writeFull( OutputStream *ostream ) const
  {
    Object::writeFull( ostream );

    ostream->writeInt( parent );
    ostream->writeVec3( velocity );
    ostream->writeVec3( momentum );
    ostream->writeVec3( floor );
    ostream->writeInt( lower );
    ostream->writeFloat( depth );
  }

  void DynObject::readUpdate( InputStream *istream )
  {
    Object::readUpdate( istream );

    p        = istream->readVec3();
    velocity = istream->readVec3();
    momentum = istream->readVec3();
  }

  void DynObject::writeUpdate( OutputStream *ostream ) const
  {
    Object::writeUpdate( ostream );

    ostream->writeVec3( p );
    ostream->writeFloat( life );
    ostream->writeVec3( velocity );
    ostream->writeVec3( momentum );
  }

}
