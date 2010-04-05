/*
 *  Math.h
 *
 *  Replacement for math.h and some other utility functions
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING for details.
 */

#pragma once

#ifndef OZ_GNU_MATH

#include <cmath>

#define __builtin_fabsf( x )            fabsf( x )
#define __builtin_floorf( x )           floorf( x )
#define __builtin_ceilf( x )            ceilf( x )
#define __builtin_roundf( x )           roundf( x )
#define __builtin_fmodf( x, y )         fmodf( x, y )
#define __builtin_sqrtf( x )            sqrtf( x )
#define __builtin_expf( x )             expf( x )
#define __builtin_logf( x )             logf( x )
#define __builtin_powf( x, y )          powf( x, y )
#define __builtin_sinf( x )             sinf( x )
#define __builtin_cosf( x )             cosf( x )
#define __builtin_sincosf( x, s, c )    sincosf( x, s, c )
#define __builtin_tanf( x )             tanf( x )
#define __builtin_asinf( x )            asinf( x )
#define __builtin_acosf( x )            acosf( x )
#define __builtin_atanf( x )            atanf( x )
#define __builtin_atan2f( x, y )        atan2f( x, y )
#define __builtin_nanf( x )             nanf( x )
#define __builtin_inff()                INFINITY
#define __builtin_isnanf( x )           isnanf( x )
#define __builtin_isinff( x )           isinff( x )

#ifdef OZ_MSVC

#define roundf( x )     ( ( x ) < 0.0f ? ceilf( ( x ) - 0.5f ) : floorf( ( x ) + 0.5f ) )
#define nanf( x )       ( HUGE_VAL * 0.0f )
#define INFINITY        HUGE_VAL
#define isnanf( x )     _isnanf( x )
#define isinff( x )     ( !_isnanf( x ) && !_finite( x ) )

#endif

#endif

namespace oz
{

  class Math
  {
    private:

      // static class
      Math() {}
      Math( const Math& );

    public:

      static const float FLOAT_EPS;
      static const float E;
      static const float LOG2E;
      static const float LOG10E;
      static const float LN2;
      static const float LN10;
      static const float PI;
      static const float PI_2;
      static const float PI_4;
      static const float _1_PI;
      static const float _2_PI;
      static const float _2_SQRTPI;
      static const float SQRT2;
      static const float SQRT1_2;

      static const int  INT_MAX  = ~0u >> 1;
      static const long LONG_MAX = ~0ul >> 1;

      static const float MAX_RAND;

      /*
       * Standard math functions
       */

      static float abs( float x )
      {
        return __builtin_fabsf( x );
      }

      static float floor( float x )
      {
        return __builtin_floorf( x );
      }

      static float ceil( float x )
      {
        return __builtin_ceilf( x );
      }

      static float round( float x )
      {
        return __builtin_roundf( x );
      }

      static float mod( float x, float y )
      {
        return __builtin_fmodf( x, y );
      }

      static float sqrt( float x )
      {
        return __builtin_sqrtf( x );
      }

      static float exp( float x )
      {
        return __builtin_expf( x );
      }

      static float log( float x )
      {
        return __builtin_logf( x );
      }

      static float pow( float x, float y )
      {
        return __builtin_powf( x, y );
      }

      static float sin( float x )
      {
        return __builtin_sinf( x );
      }

      static float cos( float x )
      {
        return __builtin_cosf( x );
      }

      static void sincos( float x, float* s, float* c )
      {
#ifdef OZ_HAVE_SINCOSF
        __builtin_sincosf( x, s, c );
#else
        *s = __builtin_sinf( x );
        *c = __builtin_cosf( x );
#endif
      }

      static float tan( float x )
      {
        return __builtin_tanf( x );
      }

      static float asin( float x )
      {
        return __builtin_asinf( x );
      }

      static float acos( float x )
      {
        return __builtin_acosf( x );
      }

      static float atan( float x )
      {
        return __builtin_atanf( x );
      }

      static float atan2( float x, float y )
      {
        return __builtin_atan2f( x, y );
      }

      /*
       * Some additional functions
       */

      static float nan()
      {
        return __builtin_nanf( "0" );
      }

      static float inf()
      {
        return __builtin_inff();
      }

      static bool isNaN( float x )
      {
        return __builtin_isnanf( x );
      }

      static bool isInf( float x )
      {
        return __builtin_isinff( x );
      }

      static float sgn( float x )
      {
        return x < 0.0f ? -1.0f : x > 0.0f ? 1.0f : 0.0f;
      }

      static float lerp( float t, float a, float b )
      {
        return a + t * ( b - a );
      }

      // deg-to-rad and rad-to-deg conversion
      static float rad( float x )
      {
        return x * ( PI / 180.0f );
      }

      static float deg( float x )
      {
        return x * ( _1_PI * 180.0f );
      }

      static int toBits( float x )
      {
        union FloatBits
        {
          float f;
          int   b;
        };
        FloatBits fb = { x };
        return fb.b;
      }

      static float fromBits( int i )
      {
        union BitsFloat
        {
          int   b;
          float f;
        };
        BitsFloat fb = { i };
        return fb.f;
      }

      // fast inverse sqrt that appeared in Quake source (google for detailed explanations)
      static float fInvSqrt( float x )
      {
        float y = fromBits( 0x5f3759df - ( toBits( x ) >> 1 ) );
        return y * ( 1.5f - 0.5f * x * y*y );
      }

      // is power of two?
      template <typename Value>
      static bool isPow2( const Value& v )
      {
        return v & ( v - 1 ) == 0;
      }

      // ponter to srand in stdlib.h
      static void ( *const seed )( uint seed );

      // random integer between 0 and RAND_MAX == INT_MAX
      // (pointer to rand() function in stdlib.h)
      static int ( *const rand )();

      // random integer between 0 and max - 1
      static int randn( int max )
      {
        return rand() % max;
      }

      // random float from interval [0, 1]
      static float frand();

  };

}
