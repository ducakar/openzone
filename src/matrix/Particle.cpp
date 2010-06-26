/*
 *  Particle.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING file for details.
 */

#include "stable.hpp"

#include "matrix/Particle.hpp"

namespace oz
{

  const float Particle::MAX_ROTVELOCITY = 400.0f * Timer::TICK_TIME;
  const float Particle::DAMAGE_THRESHOLD = 50.0f;

  Pool<Particle> Particle::pool;

  void Particle::readFull( InputStream* istream )
  {
    p           = istream->readVec3();
    velocity    = istream->readVec3();

    colour      = istream->readVec3();
    rot         = istream->readVec3();
    rotVelocity = istream->readVec3();

    restitution   = istream->readFloat();
    mass        = istream->readFloat();
    lifeTime    = istream->readFloat();
  }

  void Particle::writeFull( OutputStream* ostream )
  {
    ostream->writeVec3( p );
    ostream->writeVec3( velocity );

    ostream->writeVec3( colour );
    ostream->writeVec3( rot );
    ostream->writeVec3( rotVelocity );

    ostream->writeFloat( restitution );
    ostream->writeFloat( mass );
    ostream->writeFloat( lifeTime );
  }

  void Particle::readUpdate( InputStream* istream )
  {
    p        = istream->readVec3();
    velocity = istream->readVec3();
  }

  void Particle::writeUpdate( OutputStream* ostream )
  {
    ostream->writeVec3( p );
    ostream->writeVec3( velocity );
  }

}
