/*
 *  Particle.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING for details.
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
    p           = istream->readVec4();
    velocity    = istream->readVec4();

    color       = istream->readVec4();
    rot         = istream->readVec4();
    rotVelocity = istream->readVec4();

    restitution   = istream->readFloat();
    mass        = istream->readFloat();
    lifeTime    = istream->readFloat();
  }

  void Particle::writeFull( OutputStream* ostream )
  {
    ostream->writeVec4( p );
    ostream->writeVec4( velocity );

    ostream->writeVec4( color );
    ostream->writeVec4( rot );
    ostream->writeVec4( rotVelocity );

    ostream->writeFloat( restitution );
    ostream->writeFloat( mass );
    ostream->writeFloat( lifeTime );
  }

  void Particle::readUpdate( InputStream* istream )
  {
    p        = istream->readVec4();
    velocity = istream->readVec4();
  }

  void Particle::writeUpdate( OutputStream* ostream )
  {
    ostream->writeVec4( p );
    ostream->writeVec4( velocity );
  }

}
