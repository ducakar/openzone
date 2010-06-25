/*
 *  Bounds.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING for details.
 */

#pragma once

namespace oz
{

  class AABB;

  class Bounds
  {
    public:

      Vec3 mins;
      Vec3 maxs;

      explicit Bounds()
      {}

      explicit Bounds( const Vec3& mins_, const Vec3& maxs_ ) : mins( mins_ ), maxs( maxs_ )
      {}

      Bounds& fromPointMove( const Vec3& p, const Vec3& move, float eps = 0.0f )
      {
        if( move.x < 0.0f ) {
          mins.x = p.x - 2.0f * eps + move.x;
          maxs.x = p.x + 2.0f * eps;
        }
        else {
          mins.x = p.x - 2.0f * eps;
          maxs.x = p.x + 2.0f * eps + move.x;
        }
        if( move.y < 0.0f ) {
          mins.y = p.y - 2.0f * eps + move.y;
          maxs.y = p.y + 2.0f * eps;
        }
        else {
          mins.y = p.y - 2.0f * eps;
          maxs.y = p.y + 2.0f * eps + move.y;
        }
        if( move.z < 0.0f ) {
          mins.z = p.z - 2.0f * eps + move.z;
          maxs.z = p.z + 2.0f * eps;
        }
        else {
          mins.z = p.z - 2.0f * eps;
          maxs.z = p.z + 2.0f * eps + move.z;
        }
        return *this;
      }

      Bounds operator + ( const Vec3& v ) const
      {
        return Bounds( mins + v, maxs + v );
      }

      Bounds operator - ( const Vec3& v ) const
      {
        return Bounds( mins - v, maxs - v );
      }

      Bounds& operator += ( const Vec3& v )
      {
        mins += v;
        maxs += v;
        return *this;
      }

      Bounds& operator -= ( const Vec3& v )
      {
        mins -= v;
        maxs -= v;
        return *this;
      }

      bool includes( const Vec3& v, float eps = 0.0f ) const
      {
        return
            mins.x - eps <= v.x && v.x <= maxs.x + eps &&
            mins.y - eps <= v.y && v.y <= maxs.y + eps &&
            mins.z - eps <= v.z && v.z <= maxs.z + eps;
      }

      bool isInside( const Bounds& b, float eps = 0.0f ) const
      {
        return
            b.mins.x - eps <= mins.x && maxs.x <= b.maxs.x + eps &&
            b.mins.y - eps <= mins.y && maxs.y <= b.maxs.y + eps &&
            b.mins.z - eps <= mins.z && maxs.z <= b.maxs.z + eps;
      }

      bool includes( const Bounds& b, float eps = 0.0f ) const
      {
        return b.isInside( *this, eps );
      }

      bool overlaps( const Bounds& b, float eps = 0.0f ) const
      {
        return
            b.mins.x - eps <= maxs.x && mins.x <= b.maxs.x + eps &&
            b.mins.y - eps <= maxs.y && mins.y <= b.maxs.y + eps &&
            b.mins.z - eps <= maxs.z && mins.z <= b.maxs.z + eps;
      }

      // implemented in AABB.hpp
      AABB toAABB( float eps = 0.0f ) const;
      bool isInside( const AABB& a, float eps = 0.0f ) const;
      bool includes( const AABB& a, float eps = 0.0f ) const;
      bool overlaps( const AABB& a, float eps = 0.0f ) const;

  };

}
