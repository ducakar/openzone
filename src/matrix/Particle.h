/*
 *  Particle.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

#include "Timer.h"

namespace oz
{

  struct Sector;

  // should NOT be virtual (offset of .p must be 0 otherwise a hack in Matrix::Collider.test(L)Ray
  // won't work)
  class Particle
  {
    public:

      static const float MAX_ROTVELOCITY;

      /*
       *  FIELDS
       */

      Vec3      p;            // position

      int       index;        // position in world.objects vector
      Sector    *sector;

      Particle  *prev[1];
      Particle  *next[1];

      Vec3      velocity;

      float     rejection;    // 1.0 < reject < 2.0
      float     mass;
      float     lifeTime;

      // graphics data
      float     size;
      Vec3      color;
      Vec3      rot;
      Vec3      rotVelocity;

      Particle()
      {}

      Particle( const Vec3 &p_, const Vec3 &velocity_, float rejection_, float mass_,
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

      // serialize whole object
      virtual void serialize( char *stream ) const
      {
        *stream = 0;
      }

      // serialize only information necessary for network synchronizing
      virtual void updateSerialize( char *stream ) const
      {
        *stream = 0;
      }

      // update object with recieved synchronization information from network
      virtual void updateDeserialize( const char *stream )
      {
        assert( stream != null );
      }
  };

}
