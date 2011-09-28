/*
 *  Math.hpp
 *
 *  Replacement for math.h and some other utility functions
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "Pair.hpp"

namespace oz
{

  class Math
  {
    public:

      static const float EPSILON;
      static const float E;
      static const float TAU;

      static const float INF;
      static const float NaN;

    private:

      // singleton
      Math();

    public:

      OZ_ALWAYS_INLINE
      static float abs( float x )
      {
        return __builtin_fabsf( x );
      }

      OZ_ALWAYS_INLINE
      static float floor( float x )
      {
        return __builtin_floorf( x );
      }

      OZ_ALWAYS_INLINE
      static float ceil( float x )
      {
        return __builtin_ceilf( x );
      }

      OZ_ALWAYS_INLINE
      static float round( float x )
      {
        return __builtin_roundf( x );
      }

      OZ_ALWAYS_INLINE
      static float trunc( float x )
      {
        return __builtin_truncf( x );
      }

      OZ_ALWAYS_INLINE
      static float mod( float x, float y )
      {
        return __builtin_fmodf( x, y );
      }

      OZ_ALWAYS_INLINE
      static Pair<float> fract( float x )
      {
        float integral;
        float fractional = __builtin_modff( x, &integral );
        return Pair<float>( integral, fractional );
      }

      OZ_ALWAYS_INLINE
      static float sqrt( float x )
      {
        return __builtin_sqrtf( x );
      }

      OZ_ALWAYS_INLINE
      static float exp( float x )
      {
        return __builtin_expf( x );
      }

      OZ_ALWAYS_INLINE
      static float log( float x )
      {
        return __builtin_logf( x );
      }

      OZ_ALWAYS_INLINE
      static float pow( float x, float y )
      {
        return __builtin_powf( x, y );
      }

      OZ_ALWAYS_INLINE
      static float sin( float x )
      {
        return __builtin_sinf( x );
      }

      OZ_ALWAYS_INLINE
      static float cos( float x )
      {
        return __builtin_cosf( x );
      }

      OZ_ALWAYS_INLINE
      static void sincos( float x, float* s, float* c )
      {
#if defined( OZ_HAVE_SINCOSF ) && !defined( __clang__ )
        __builtin_sincosf( x, s, c );
#else
        *s = __builtin_sinf( x );
        *c = __builtin_cosf( x );
#endif
      }

      OZ_ALWAYS_INLINE
      static float tan( float x )
      {
        return __builtin_tanf( x );
      }

      OZ_ALWAYS_INLINE
      static float asin( float x )
      {
        return __builtin_asinf( x );
      }

      OZ_ALWAYS_INLINE
      static float acos( float x )
      {
        return __builtin_acosf( x );
      }

      OZ_ALWAYS_INLINE
      static float atan( float x )
      {
        return __builtin_atanf( x );
      }

      OZ_ALWAYS_INLINE
      static float atan2( float x, float y )
      {
        return __builtin_atan2f( x, y );
      }

      // don't work with -ffast-math
      /*
      OZ_ALWAYS_INLINE
      static bool isNaN( float x )
      {
        return __builtin_isnan( x );
      }

      OZ_ALWAYS_INLINE
      static bool isFinite( float x )
      {
        return __builtin_isfinite( x );
      }

      OZ_ALWAYS_INLINE
      static int isInf( float x )
      {
        return __builtin_ininf( x );
      }

      OZ_ALWAYS_INLINE
      static bool isNormal( float x )
      {
        return __builtin_isnormal( x );
      }
      */

      OZ_ALWAYS_INLINE
      static int isInf( float x )
      {
        return x == 2.0f * x;
      }

      OZ_ALWAYS_INLINE
      static float sgn( float x )
      {
        return x < 0.0f ? -1.0f : ( x > 0.0f ? 1.0f : 0.0f );
      }

      // deg-to-rad and rad-to-deg conversion
      OZ_ALWAYS_INLINE
      static float rad( float x )
      {
        return x * ( TAU / 360.0f );
      }

      OZ_ALWAYS_INLINE
      static float deg( float x )
      {
        return x * ( 360.0f / TAU );
      }

      OZ_ALWAYS_INLINE
      static int toBits( float x )
      {
        union FloatToBits
        {
          float f;
          int   b;
        };
        FloatToBits fb = { x };
        return fb.b;
      }

      OZ_ALWAYS_INLINE
      static float fromBits( int i )
      {
        union BitsToFloat
        {
          int   b;
          float f;
        };
        BitsToFloat bf = { i };
        return bf.f;
      }

      OZ_ALWAYS_INLINE
      static float fastSqrt( float x )
      {
        return x * fastInvSqrt( x );
      }

      // fast inverse sqrt from Quake source (google for detailed explanations)
      OZ_ALWAYS_INLINE
      static float fastInvSqrt( float x )
      {
        float y = fromBits( 0x5f3759df - ( toBits( x ) >> 1 ) );
        return y * ( 1.5f - 0.5f * x * y*y );
      }

      // is power of two?
      template <typename Value>
      OZ_ALWAYS_INLINE
      static bool isPow2( const Value& v )
      {
        return ( v & ( v - 1 ) ) == 0;
      }

      // seed for random generator
      static void seed( int n );

      // random integer between 0 and max - 1
      static int rand( int max );

      // random float from interval [0, 1]
      static float rand();

  };

}
