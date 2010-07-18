/*
 *  Particle.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "matrix/common.hpp"
#include "matrix/Timer.hpp"

namespace oz
{

  struct Cell;
  struct Hit;

  class Particle
  {
    friend class DList<Particle>;
    friend class Pool<Particle>;

    public:

      static const float MAX_ROTVELOCITY;
      static const float DAMAGE_THRESHOLD;

      static Pool<Particle> pool;

      /*
       *  FIELDS
       */

      Particle* prev[1];
      Particle* next[1];

      Vec3      p;            // position

      int       index;        // position in world.objects vector
      Cell*     cell;

      Vec3      velocity;

      // graphics data
      Vec3      colour;
      Vec3      rot;
      Vec3      rotVelocity;

      float     restitution;    // 1.0 < restitution < 2.0
      float     mass;
      float     lifeTime;

      explicit Particle() : index( -1 ), cell( null )
      {}

      explicit Particle( int index_, const Vec3& p_, const Vec3& velocity_, const Vec3& color_,
                         float restitution_, float mass_, float lifeTime_ ) :
          p( p_ ), index( index_ ), cell( null ), velocity( velocity_ ), colour( color_ ),
          rot( Vec3( Math::frand() * 360.0f, Math::frand() * 360.0f, Math::frand() * 360.0f ) ),
          rotVelocity( Vec3( Math::frand() * MAX_ROTVELOCITY,
                             Math::frand() * MAX_ROTVELOCITY,
                             Math::frand() * MAX_ROTVELOCITY ) ),
          restitution( restitution_ ), mass( mass_ ), lifeTime( lifeTime_ )
      {
        assert( 1.0f + EPSILON < restitution && restitution < 2.0f - EPSILON );
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
       *  SERIALISATION
       */

      void readFull( InputStream* istream );
      void writeFull( OutputStream* ostream );
      void readUpdate( InputStream* istream );
      void writeUpdate( OutputStream* ostream );


    OZ_STATIC_POOL_ALLOC( pool )

  };

}
