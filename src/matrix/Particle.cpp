/*
 *  Particle.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#include "precompiled.h"

#include "Particle.h"

namespace oz
{

  const float Particle::MAX_ROTVELOCITY = 1000.0f;

  void Particle::readFull( InputStream *istream )
  {
    p           = istream->readVec3();
    velocity    = istream->readVec3();

    rejection   = istream->readFloat();
    mass        = istream->readFloat();
    lifeTime    = istream->readFloat();

    size        = istream->readFloat();
    color       = istream->readVec3();
    rot         = istream->readVec3();
    rotVelocity = istream->readVec3();
  }

  void Particle::writeFull( OutputStream *ostream )
  {
    ostream->writeVec3( p );
    ostream->writeVec3( velocity );

    ostream->writeFloat( rejection );
    ostream->writeFloat( mass );
    ostream->writeFloat( lifeTime );

    ostream->writeFloat( size );
    ostream->writeVec3( color );
    ostream->writeVec3( rot );
    ostream->writeVec3( rotVelocity );
  }

  void Particle::readUpdate( InputStream *istream )
  {
    p        = istream->readVec3();
    velocity = istream->readVec3();
  }

  void Particle::writeUpdate( OutputStream *ostream )
  {
    ostream->writeVec3( p );
    ostream->writeVec3( velocity );
  }

}
