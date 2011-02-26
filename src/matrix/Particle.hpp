/*
 *  Particle.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

#include "matrix/common.hpp"
#include "matrix/Timer.hpp"

namespace oz
{

  struct Cell;
  struct Hit;

  class Particle
  {
    public:

      static const float MAX_ROTVELOCITY;
      static const float DAMAGE_THRESHOLD;

      static Pool<Particle, 1024> pool;

      /*
       *  FIELDS
       */

      Particle* prev[1];
      Particle* next[1];

      int       index;        // position in world.objects vector
      Cell*     cell;

      Point3    p;            // position
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

      explicit Particle( int index, const Point3& p, const Vec3& velocity, const Vec3& colour,
                         float restitution, float mass, float lifeTime );

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
