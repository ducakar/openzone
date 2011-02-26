/*
 *  Particle.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "matrix/Particle.hpp"

namespace oz
{

  const float Particle::MAX_ROTVELOCITY = 400.0f * Timer::TICK_TIME;
  const float Particle::DAMAGE_THRESHOLD = 50.0f;

  Pool<Particle, 1024> Particle::pool;

  Particle::Particle( int index_, const Point3& p_, const Vec3& velocity_, const Vec3& colour_,
                      float restitution_, float mass_, float lifeTime_ ) :
      index( index_ ), cell( null ), p( p_ ), velocity( velocity_ ), colour( colour_ ),
      rot( Vec3( Math::frand() * 360.0f, Math::frand() * 360.0f, Math::frand() * 360.0f ) ),
      rotVelocity( Vec3( Math::frand() * MAX_ROTVELOCITY,
                          Math::frand() * MAX_ROTVELOCITY,
                          Math::frand() * MAX_ROTVELOCITY ) ),
      restitution( restitution_ ), mass( mass_ ), lifeTime( lifeTime_ )
  {
    hard_assert( 1.0f + EPSILON < restitution && restitution < 2.0f - EPSILON );
    hard_assert( mass >= 0.0f );
    hard_assert( lifeTime > 0.0f );
  }

  void Particle::readFull( InputStream* istream )
  {
    p           = istream->readPoint3();
    velocity    = istream->readVec3();

    colour      = istream->readVec3();
    rot         = istream->readVec3();
    rotVelocity = istream->readVec3();

    restitution = istream->readFloat();
    mass        = istream->readFloat();
    lifeTime    = istream->readFloat();
  }

  void Particle::writeFull( OutputStream* ostream )
  {
    ostream->writePoint3( p );
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
    p        = istream->readPoint3();
    velocity = istream->readVec3();
  }

  void Particle::writeUpdate( OutputStream* ostream )
  {
    ostream->writePoint3( p );
    ostream->writeVec3( velocity );
  }

}
