/*
 *  Math.hpp
 *
 *  Replacement for math.h and some other utility functions
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "Pair.hpp"

/*
 * Standard math header, if we don't use built-in math
 */
#ifndef OZ_BUILTIN_MATH
# include <cmath>
#endif

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

      static const float MAX_RAND;

      Math() = delete;

      OZ_ALWAYS_INLINE
      static float abs( float x )
      {
#ifdef OZ_BUILTIN_MATH
        return __builtin_fabsf( x );
#else
        return fabsf( x );
#endif
      }

      OZ_ALWAYS_INLINE
      static float floor( float x )
      {
#ifdef OZ_BUILTIN_MATH
        return __builtin_floorf( x );
#else
        return floorf( x );
#endif
      }

      OZ_ALWAYS_INLINE
      static float ceil( float x )
      {
#ifdef OZ_BUILTIN_MATH
        return __builtin_ceilf( x );
#else
        return ceilf( x );
#endif
      }

      OZ_ALWAYS_INLINE
      static float round( float x )
      {
#ifdef OZ_BUILTIN_MATH
# ifdef __clang__
        return x < 0.0f ? __builtin_ceilf( x - 0.5f ) : __builtin_floorf( x + 0.5f );
# else
        return __builtin_roundf( x );
# endif
#else
        return roundf( x );
#endif
      }

      OZ_ALWAYS_INLINE
      static float trunc( float x )
      {
#ifdef OZ_BUILTIN_MATH
# ifdef __clang__
        return x < 0.0f ? __builtin_ceilf( x ) : __builtin_floorf( x );
# else
        return __builtin_truncf( x );
# endif
#else
        return truncf( x );
#endif
      }

      OZ_ALWAYS_INLINE
      static float mod( float x, float y )
      {
#ifdef OZ_BUILTIN_MATH
        return __builtin_fmodf( x, y );
#else
        return fmodf( x, y );
#endif
      }

      OZ_ALWAYS_INLINE
      static Pair<float> fract( float x )
      {
        float integral;
#ifdef OZ_BUILTIN_MATH
        float fractional = __builtin_modff( x, &integral );
#else
        float fractional = modff( x, &integral );
#endif
        return Pair<float>( integral, fractional );
      }

      OZ_ALWAYS_INLINE
      static float sqrt( float x )
      {
#ifdef OZ_BUILTIN_MATH
        return __builtin_sqrtf( x );
#else
        return sqrtf( x );
#endif
      }

      OZ_ALWAYS_INLINE
      static float exp( float x )
      {
#ifdef OZ_BUILTIN_MATH
        return __builtin_expf( x );
#else
        return expf( x );
#endif
      }

      OZ_ALWAYS_INLINE
      static float log( float x )
      {
#ifdef OZ_BUILTIN_MATH
        return __builtin_logf( x );
#else
        return logf( x );
#endif
      }

      OZ_ALWAYS_INLINE
      static float pow( float x, float y )
      {
#ifdef OZ_BUILTIN_MATH
        return __builtin_powf( x, y );
#else
        return powf( x, y );
#endif
      }

      OZ_ALWAYS_INLINE
      static float sin( float x )
      {
#ifdef OZ_BUILTIN_MATH
        return __builtin_sinf( x );
#else
        return sinf( x );
#endif
      }

      OZ_ALWAYS_INLINE
      static float cos( float x )
      {
#ifdef OZ_BUILTIN_MATH
        return __builtin_cosf( x );
#else
        return cosf( x );
#endif
      }

      OZ_ALWAYS_INLINE
      static void sincos( float x, float* s, float* c )
      {
#ifdef OZ_BUILTIN_MATH
# if defined( OZ_HAVE_SINCOSF ) && !defined( __clang__ )
        __builtin_sincosf( x, s, c );
# else
        *s = __builtin_sinf( x );
        *c = __builtin_cosf( x );
# endif
#else
# ifdef OZ_HAVE_SINCOSF
        sincosf( x, s, c );
# else
        *s = sinf( x );
        *c = cosf( x );
# endif
#endif
      }

      OZ_ALWAYS_INLINE
      static float tan( float x )
      {
#ifdef OZ_BUILTIN_MATH
        return __builtin_tanf( x );
#else
        return tanf( x );
#endif
      }

      OZ_ALWAYS_INLINE
      static float asin( float x )
      {
#ifdef OZ_BUILTIN_MATH
        return __builtin_asinf( x );
#else
        return asinf( x );
#endif
      }

      OZ_ALWAYS_INLINE
      static float acos( float x )
      {
#ifdef OZ_BUILTIN_MATH
        return __builtin_acosf( x );
#else
        return acosf( x );
#endif
      }

      OZ_ALWAYS_INLINE
      static float atan( float x )
      {
#ifdef OZ_BUILTIN_MATH
        return __builtin_atanf( x );
#else
        return atanf( x );
#endif
      }

      OZ_ALWAYS_INLINE
      static float atan2( float x, float y )
      {
#ifdef OZ_BUILTIN_MATH
        return __builtin_atan2f( x, y );
#else
        return atan2f( x, y );
#endif
      }

      OZ_ALWAYS_INLINE
      static bool isNaN( float x )
      {
#ifdef OZ_BUILTIN_MATH
        return __builtin_isnan( x ) || x == x + 1;
#else
        return std::isnan( x ) || x == x + 1;
#endif
      }

      OZ_ALWAYS_INLINE
      static bool isFinite( float x )
      {
#ifdef OZ_BUILTIN_MATH
        return __builtin_isfinite( x ) && x != x + 1;
#else
        return std::isfinite( x ) && x != x + 1;
#endif
      }

      OZ_ALWAYS_INLINE
      static int isInf( float x )
      {
#ifdef OZ_BUILTIN_MATH
        return __builtin_isinf( x );
#else
        return std::isinf( x );
#endif
      }

      OZ_ALWAYS_INLINE
      static bool isNormal( float x )
      {
#ifdef OZ_BUILTIN_MATH
        return __builtin_isnormal( x );
#else
        return std::isnormal( x );
#endif
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
        BitsToFloat fb = { i };
        return fb.f;
      }

      OZ_ALWAYS_INLINE
      static float fastSqrt( float x )
      {
        return x * fastInvSqrt( x );
      }

      // famous fast inverse sqrt from Quake source (google for detailed explanations)
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

      // pointer to srand in cstdlib
      static void ( * const seed )( uint seed );

      // random integer between 0 and RAND_MAX
      // (pointer to rand() function in <cstdlib>)
      static int ( * const rand )();

      // random integer between 0 and max - 1
      OZ_ALWAYS_INLINE
      static int randn( int max )
      {
        return rand() % max;
      }

      // random float from interval [0, 1]
      static float frand();

  };

}
