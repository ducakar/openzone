/*
 *  Structure.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING for details.
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

      static Pool<Structure, 0, 256> pool;

    private:

      Structure* next[1];

    public:

      int        index;
      int        bsp;
      Vec4       p;
      Rotation   rot;
      float      life;

      explicit Structure( int index, int bsp, const Vec4& p, Rotation rot );
      explicit Structure( int index, int bsp_, InputStream* istream );

      static Bounds rotate( const Bounds& in, Rotation rot )
      {
        Bounds out;
        Vec4 p = ( in.maxs - in.mins ) * 0.5f;

        switch( rot ) {
          case Structure::R0: {
            out.mins = in.mins + p;
            out.maxs = in.maxs + p;
            break;
          }
          case Structure::R90: {
            out.mins = Vec4( -in.maxs.y + p.x, in.mins.x + p.y, in.mins.z + p.z );
            out.maxs = Vec4( -in.mins.y + p.x, in.maxs.x + p.y, in.maxs.z + p.z );
            break;
          }
          case Structure::R180: {
            out.mins = p + Vec4( -in.maxs.x, -in.maxs.y, +in.mins.z );
            out.maxs = p + Vec4( -in.mins.x, -in.mins.y, +in.maxs.z );
            break;
          }
          case Structure::R270: {
            out.mins = Vec4( in.mins.y + p.x, -in.maxs.x + p.y, in.mins.z + p.z );
            out.maxs = Vec4( in.maxs.y + p.x, -in.mins.x + p.y, in.maxs.z + p.z );
            break;
          }
          default: {
            assert( false );
            break;
          }
        }
        return out;
      }

      void setRotation( const Bounds& in, Rotation rot )
      {
        switch( rot ) {
          case Structure::R0: {
            mins = in.mins + p;
            maxs = in.maxs + p;
            break;
          }
          case Structure::R90: {
            mins = Vec4( -in.maxs.y + p.x, in.mins.x + p.y, in.mins.z + p.z );
            maxs = Vec4( -in.mins.y + p.x, in.maxs.x + p.y, in.maxs.z + p.z );
            break;
          }
          case Structure::R180: {
            mins = p + Vec4( -in.maxs.x, -in.maxs.y, +in.mins.z );
            maxs = p + Vec4( -in.mins.x, -in.mins.y, +in.maxs.z );
            break;
          }
          case Structure::R270: {
            mins = Vec4( in.mins.y + p.x, -in.maxs.x + p.y, in.mins.z + p.z );
            maxs = Vec4( in.maxs.y + p.x, -in.mins.x + p.y, in.maxs.z + p.z );
            break;
          }
          default: {
            assert( false );
            break;
          }
        }
      }

      void damage( float damage )
      {
        damage -= DAMAGE_THRESHOLD;

        if( damage > 0.0f ) {
          life -= damage;

          if( life <= 0.0f ) {
            destroy();
          }
        }
      }

      void destroy();

      void readFull( InputStream* istream );
      void writeFull( OutputStream* ostream );

    OZ_STATIC_POOL_ALLOC( pool )

  };

}
