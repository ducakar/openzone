/*
 *  Particle.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

#include "io.h"
#include "Timer.h"

namespace oz
{

  struct Sector;

  class Particle : public PoolAlloc<Particle, 0>
  {
    friend class Pool<Particle, 0>;
    friend class PoolAlloc<Particle, 0>;
    friend class DList<Particle, 0>;

    private:

      Particle  *prev[1];
      Particle  *next[1];

    public:

      static const float MAX_ROTVELOCITY;

      /*
       *  FIELDS
       */

      Vec3      p;            // position

      int       index;        // position in world.objects vector
      Sector    *sector;

      Vec3      velocity;

      float     rejection;    // 1.0 < rejection < 2.0
      float     mass;
      float     lifeTime;

      // graphics data
      float     size;
      Vec3      color;
      Vec3      rot;
      Vec3      rotVelocity;

      explicit Particle()
      {}

      explicit Particle( const Vec3 &p_, const Vec3 &velocity_, float rejection_, float mass_,
                         float lifeTime_, float size_, const Vec3 &color_ ) :
          p( p_ ), velocity( velocity_ ),
          rejection( rejection_ ),
          mass( mass_ ), lifeTime( lifeTime_ ),
          size( size_ ), color( color_ ),
          rot( Vec3( Math::frand() * 360.0f, Math::frand() * 360.0f, Math::frand() * 360.0f ) ),
          rotVelocity( Vec3( Math::frand() * MAX_ROTVELOCITY,
                             Math::frand() * MAX_ROTVELOCITY,
                             Math::frand() * MAX_ROTVELOCITY ) )
      {}

      void update()
      {
        rot += rotVelocity * timer.frameTime;
      }

      /*
       *  SERIALIZATION
       */

      void readFull( InputStream *istream );
      void writeFull( OutputStream *ostream );
      void readUpdate( InputStream *istream );
      void writeUpdate( OutputStream *ostream );

  };

}
