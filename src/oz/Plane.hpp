/*
 *  Plane.hpp
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

/**
 * @file Plane.hpp
 */

#include "Point3.hpp"

namespace oz
{

/**
 * 3D plane.
 */
#ifdef OZ_SIMD
class Plane : public Simd
#else
class Plane
#endif
{
  public:

#ifndef OZ_SIMD
    float nx; ///< X component of the normal.
    float ny; ///< Y component of the normal.
    float nz; ///< Z component of the normal.
    float d;  ///< Distance from origin.
#endif

    /**
     * Create an uninitialised instance.
     */
    OZ_ALWAYS_INLINE
    Plane()
    {}

#ifdef OZ_SIMD
  protected:

    /**
     * Create from an uint SIMD vector.
     */
    OZ_ALWAYS_INLINE
    explicit Plane( uint4 u4 ) : Simd( u4 )
    {}

    /**
     * Create from a float SIMD vector.
     */
    OZ_ALWAYS_INLINE
    explicit Plane( float4 f4 ) : Simd( f4 )
    {}

  public:
#endif

    /**
     * Create form a pair of normal and distance from the origin.
     */
#ifdef OZ_SIMD
    OZ_ALWAYS_INLINE
    explicit Plane( const Vec3& n, float d ) : Simd( float4( n.x, n.y, n.z, d ) )
    {}
#else
    OZ_ALWAYS_INLINE
    explicit Plane( const Vec3& n, float d_ ) : nx( n.x ), ny( n.y ), nz( n.z ), d( d_ )
    {}
#endif

    /**
     * Create from an array of 4 floats.
     */
#ifdef OZ_SIMD
    OZ_ALWAYS_INLINE
    explicit Plane( const float* p ) : Simd( float4( p[0], p[1], p[2],  p[3] ) )
    {}
#else
    OZ_ALWAYS_INLINE
    explicit Plane( const float* p ) : nx( p[0] ), ny( p[1] ), nz( p[2] ), d( p[3] )
    {}
#endif

    /**
     * Create with the given member values.
     */
#ifdef OZ_SIMD
    OZ_ALWAYS_INLINE
    explicit Plane( float nx, float ny, float nz, float d ) : Simd( float4( nx, ny, nz, d ) )
    {}
#else
    OZ_ALWAYS_INLINE
    explicit Plane( float nx_, float ny_, float nz_, float d_ ) :
        nx( nx_ ), ny( ny_ ), nz( nz_ ), d( d_ )
    {}
#endif

    /**
     * Equality.
     */
    OZ_ALWAYS_INLINE
    bool operator == ( const Plane& p ) const
    {
      return nx == p.nx && ny == p.ny && nz == p.nz && d == p.d;
    }

    /**
     * Inequality.
     */
    OZ_ALWAYS_INLINE
    bool operator != ( const Plane& p ) const
    {
      return nx != p.nx || ny != p.ny || nz != p.nz || d != p.d;
    }

    /**
     * Constant float pointer to the members.
     */
    OZ_ALWAYS_INLINE
    operator const float* () const
    {
      return &nx;
    }

    /**
     * Float pointer to the members.
     */
    OZ_ALWAYS_INLINE
    operator float* ()
    {
      return &nx;
    }

    /**
     * Constant reference to the i-th member.
     */
    OZ_ALWAYS_INLINE
    const float& operator [] ( int i ) const
    {
      hard_assert( 0 <= i && i < 4 );

      return ( &nx )[i];
    }

    /**
     * Reference to the i-th member.
     */
    OZ_ALWAYS_INLINE
    float& operator [] ( int i )
    {
      hard_assert( 0 <= i && i < 4 );

      return ( &nx )[i];
    }

    /**
     * Normal.
     */
    OZ_ALWAYS_INLINE
    Vec3 n() const
    {
#ifdef OZ_SIMD
      return Vec3( u4 & uint4( 0xffffffff, 0xffffffff, 0xffffffff, 0x00000000 ) );
#else
      return Vec3( nx, ny, nz );
#endif
    }

    /**
     * %Plane with the same distance but normal that has absolute components.
     */
    OZ_ALWAYS_INLINE
    Plane abs() const
    {
#ifdef OZ_SIMD
      return Plane( u4 & uint4( 0x7fffffff, 0x7fffffff, 0x7fffffff, 0xffffffff ) );
#else
      return Plane( Math::abs( nx ), Math::abs( ny ), Math::abs( nz ), d );
#endif
    }

    /**
     * Projection of a vector to the plane's normal.
     */
    OZ_ALWAYS_INLINE
    friend float operator * ( const Vec3& v, const Plane& plane )
    {
      return v.x*plane.nx + v.y*plane.ny + v.z*plane.nz;
    }

    /**
     * Distance between a point and the plane.
     */
    OZ_ALWAYS_INLINE
    friend float operator * ( const Point3& p, const Plane& plane )
    {
      return p.x*plane.nx + p.y*plane.ny + p.z*plane.nz - plane.d;
    }

};

}
