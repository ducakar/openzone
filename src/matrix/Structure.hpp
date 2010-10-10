/*
 *  Structure.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "matrix/common.hpp"

namespace oz
{

  class Structure : public Bounds
  {
    friend class Pool<Structure, 0, 256>;

    private:

      static const float DAMAGE_THRESHOLD;

    public:

      enum Rotation
      {
        R0   = 0,
        R90  = 1,
        R180 = 2,
        R270 = 3
      };

      struct Entity
      {
        enum State
        {
          CLOSED,
          OPENING,
          OPENED,
          CLOSING
        };

        Vec3  offset;

        float timer;
        State state;
      };

      static Pool<Structure, 0, 256> pool;

    private:

      Structure* next[1];

    public:

      int        index;
      int        bsp;
      Vec3       p;
      Rotation   rot;
      float      life;

      DArray<Entity> entities;

      explicit Structure( int index, int bsp, const Vec3& p, Rotation rot );
      explicit Structure( int index, int bsp_, InputStream* istream );

      static Bounds rotate( const Bounds& in, Rotation rot )
      {
        Vec3 p = ( in.maxs - in.mins ) * 0.5f;

        switch( rot ) {
          case Structure::R0: {
            return Bounds( p + in.mins,
                           p + in.maxs );
          }
          case Structure::R90: {
            return Bounds( p + Vec3( -in.maxs.y, +in.mins.x, +in.mins.z ),
                           p + Vec3( -in.mins.y, +in.maxs.x, +in.maxs.z ) );
          }
          case Structure::R180: {
            return Bounds( p + Vec3( -in.maxs.x, -in.maxs.y, +in.mins.z ),
                           p + Vec3( -in.mins.x, -in.mins.y, +in.maxs.z ) );
          }
          case Structure::R270: {
            return Bounds( p + Vec3( +in.mins.y, -in.maxs.x, +in.mins.z ),
                           p + Vec3( +in.maxs.y, -in.mins.x, +in.maxs.z ) );
          }
        }
        assert( false );
      }

      void setRotation( const Bounds& in, Rotation rot )
      {
        switch( rot ) {
          case Structure::R0: {
            mins = p + in.mins;
            maxs = p + in.maxs;
            break;
          }
          case Structure::R90: {
            mins = p + Vec3( -in.maxs.y, in.mins.x, in.mins.z );
            maxs = p + Vec3( -in.mins.y, in.maxs.x, in.maxs.z );
            break;
          }
          case Structure::R180: {
            mins = p + Vec3( -in.maxs.x, -in.maxs.y, +in.mins.z );
            maxs = p + Vec3( -in.mins.x, -in.mins.y, +in.maxs.z );
            break;
          }
          case Structure::R270: {
            mins = p + Vec3( in.mins.y, -in.maxs.x, in.mins.z );
            maxs = p + Vec3( in.maxs.y, -in.mins.x, in.maxs.z );
            break;
          }
        }
      }

      void damage( float damage )
      {
        damage -= DAMAGE_THRESHOLD;

        if( damage > 0.0f ) {
          life -= damage;
        }
      }

      void destroy();
      void update();

      void readFull( InputStream* istream );
      void writeFull( OutputStream* ostream );

    OZ_STATIC_POOL_ALLOC( pool )

  };

}
