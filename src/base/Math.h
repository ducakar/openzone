/*
 *  Math.h
 *
 *  Replacement for math.h and some other utility functions
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

namespace oz
{

  class Math
  {
    private:

      // static class
      Math();
      Math( const Math& );

      static const Math math;

    public:

      static const float FLOAT_EPS;
      static const float DOUBLE_EPS;
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
      static const float NaN;
      static const float INF;

      static const int  INT_MAX  = ~0u >> 1;
      static const long LONG_MAX = ~0ul >> 1;

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

      static void sincos( float x, float *s, float *c )
      {
        // FreeBSD libc doesn't have sincos function
#ifdef HAVE_SINCOS
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

      // returns true, if value is not a number
      static bool isNaN( float x )
      {
        return __builtin_isnanf( x );
      }

      static float sgn( float x )
      {
        return x < 0.0f ? -1.0f : x > 0.0f ? 1.0f : 0.0f;
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

      static const int &toBits( const float &f )
      {
        return *(int*) &f;
      }

      static const float &fromBits( const int &b )
      {
        return *(float*) &b;
      }

      /**
       * Is power of two?
       */
      template <class Value>
      static bool isPow2( const Value &v )
      {
        return v & ( v - 1 ) == 0;
      }

      // random integer from 0 to RAND_MAX == INT_MAX
      // (pointer to rand() function in stdlib.h)
      static int ( *const rand )();

      // random float from interval [0, 1]
      static float frand()
      {
        return (float) rand() / (float) INT_MAX;
      }

  };

}
