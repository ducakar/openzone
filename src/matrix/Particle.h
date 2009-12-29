/*
 *  Particle.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING for details.
 */

#pragma once

#include "matrix.h"
#include "Timer.h"

namespace oz
{

  struct Cell;
  struct Hit;

  struct Particle
  {
    friend class Pool<Particle>;
    friend class DList<Particle>;

    private:

      static const float MAX_ROTVELOCITY;
      static const float DAMAGE_THRESHOLD = 50.0f;

      Particle* prev[1];
      Particle* next[1];

    public:

      static Pool<Particle> pool;

      /*
       *  FIELDS
       */

      Vec3      p;            // position

      int       index;        // position in world.objects vector
      Cell*     cell;

      Vec3      velocity;

      // graphics data
      Vec3      color;
      Vec3      rot;
      Vec3      rotVelocity;

      float     rejection;    // 1.0 < rejection < 2.0
      float     mass;
      float     lifeTime;

      explicit Particle() : index( -1 ), cell( null )
      {}

      explicit Particle( int index_, const Vec3& p_, const Vec3& velocity_, const Vec3& color_,
                         float rejection_, float mass_, float lifeTime_ ) :
          p( p_ ), index( index_ ), cell( null ), velocity( velocity_ ), color( color_ ),
          rot( Vec3( Math::frand() * 360.0f, Math::frand() * 360.0f, Math::frand() * 360.0f ) ),
          rotVelocity( Vec3( Math::frand() * MAX_ROTVELOCITY,
                             Math::frand() * MAX_ROTVELOCITY,
                             Math::frand() * MAX_ROTVELOCITY ) ),
          rejection( rejection_ ), mass( mass_ ), lifeTime( lifeTime_ )
      {
        assert( 1.0f + EPSILON < rejection && rejection < 2.0f - EPSILON );
        assert( mass >= 0.0f );
        assert( lifeTime > 0.0f );
      }

      void damage( float damage )
      {
        damage -= DAMAGE_THRESHOLD;

        if( damage > 0.0f ) {
          lifeTime = 0.0f;
        }
      }

      void update()
      {
        rot += rotVelocity;
      }

      /*
       *  SERIALIZATION
       */

      void readFull( InputStream* istream );
      void writeFull( OutputStream* ostream );
      void readUpdate( InputStream* istream );
      void writeUpdate( OutputStream* ostream );


    OZ_STATIC_POOL_ALLOC( pool );

  };

}
