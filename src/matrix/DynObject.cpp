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

  void DynObject::onDestroy()
  {
    synapse.genParts( type->nDebris, p, Vec3::zero(), type->debrisVelocitySpread,
                      type->debrisRejection, type->debrisMass, type->debrisLifeTime,
                      type->debrisColor, type->debrisColorSpread );
  }

  void DynObject::readFull( InputStream *istream )
  {
    Object::readFull( istream );

    velocity   = istream->readVec3();
    momentum   = istream->readVec3();
    floor      = istream->readVec3();
    lower      = istream->readInt();
    waterDepth = istream->readFloat();
  }

  void DynObject::writeFull( OutputStream *ostream ) const
  {
    Object::writeFull( ostream );

    ostream->writeVec3( velocity );
    ostream->writeVec3( momentum );
    ostream->writeVec3( floor );
    ostream->writeInt( lower );
    ostream->writeFloat( waterDepth );
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
