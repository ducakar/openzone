/*
 * libozdyn - OpenZone Dynamics Library.
 *
 * Copyright © 2002-2012 Davorin Učakar
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from
 * the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software in
 *    a product, an acknowledgement in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

/**
 * @file ozdyn/collision/Transf.hh
 */

#pragma once

#include <oz/oz.hh>

namespace oz
{

class Transf
{
  public:

    /// Identity transformation.
    static const Transf ID;

    Vec3  a;
    Vec3  b;
    Vec3  c;
    Point p;

  public:

    OZ_ALWAYS_INLINE
    explicit Transf() = default;

    OZ_ALWAYS_INLINE
    explicit Transf( const Vec3& a_, const Vec3& b_, const Vec3& c_, const Point& p_ ) :
      a( a_ ), b( b_ ), c( c_ ), p( p_ )
    {}

    /**
     * Equality.
     */
    OZ_ALWAYS_INLINE
    bool operator == ( const Transf& t ) const
    {
      return a == t.a && b == t.b && c == t.c && p == t.p;
    }

    /**
     * Inequality.
     */
    OZ_ALWAYS_INLINE
    bool operator != ( const Transf& t ) const
    {
      return a != t.a || b != t.b || c != t.c || p != t.p;
    }

    OZ_ALWAYS_INLINE
    operator const float* () const
    {
      return &a.x;
    }

    OZ_ALWAYS_INLINE
    operator float* () const
    {
      return &a.x;
    }

    OZ_ALWAYS_INLINE
    const float& operator [] ( int i ) const
    {
      hard_assert( 0 <= i && i < 12 );

      return ( &a.x )[i];
    }

    OZ_ALWAYS_INLINE
    float& operator [] ( int i )
    {
      hard_assert( 0 <= i && i < 12 );

      return ( &a.x )[i];
    }

};

}
