/*
 *  Math.h
 *
 *  Replacement for math.h and some other utility functions
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

#ifdef WIN32
#include <math.h>
#endif

namespace oz
{

  class Math
  {
    private:

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
#ifdef WIN32
        return fabsf( x );
#else
        return __builtin_fabsf( x );
#endif
      }

      static float floor( float x )
      {
#ifdef WIN32
        return floorf( x );
#else
        return __builtin_floorf( x );
#endif
      }

      static float ceil( float x )
      {
#ifdef WIN32
        return ceilf( x );
#else
        return __builtin_ceilf( x );
#endif
      }

      static float round( float x )
      {
#ifdef WIN32
        return ceilf( x + 0.5f );
#else
        return __builtin_roundf( x );
#endif
      }

      static float mod( float x, float y )
      {
#ifdef WIN32
        return fmodf( x, y );
#else
        return __builtin_fmodf( x, y );
#endif
      }

      static float sqrt( float x )
      {
#ifdef WIN32
        return sqrtf( x );
#else
        return __builtin_sqrtf( x );
#endif
      }

      static float exp( float x )
      {
#ifdef WIN32
        return expf( x );
#else
        return __builtin_expf( x );
#endif
      }

      static float log( float x )
      {
#ifdef WIN32
        return logf( x );
#else
        return __builtin_logf( x );
#endif
      }

      static float pow( float x, float y )
      {
#ifdef WIN32
        return powf( x, y );
#else
        return __builtin_powf( x, y );
#endif
      }

      static float sin( float x )
      {
#ifdef WIN32
        return sinf( x );
#else
        return __builtin_sinf( x );
#endif
      }

      static float cos( float x )
      {
#ifdef WIN32
        return cosf( x );
#else
        return __builtin_cosf( x );
#endif
      }

      static void sincos( float x, float *s, float *c )
      {
        // FreeBSD libc doesn't have sincos function
#ifdef HAVE_SINCOS
        __builtin_sincosf( x, s, c );
#else

#ifdef WIN32
        *s = sinf( x );
        *c = cosf( x );
#else
        *s = __builtin_sinf( x );
        *c = __builtin_cosf( x );
#endif

#endif
      }

      static float tan( float x )
      {
#ifdef WIN32
        return tanf( x );
#else
        return __builtin_tanf( x );
#endif
      }

      static float asin( float x )
      {
#ifdef WIN32
        return asinf( x );
#else
        return __builtin_asinf( x );
#endif
      }

      static float acos( float x )
      {
#ifdef WIN32
        return cosf( x );
#else
        return __builtin_acosf( x );
#endif
      }

      static float atan( float x )
      {
#ifdef WIN32
        return atanf( x );
#else
        return __builtin_atanf( x );
#endif
      }

      static float atan2( float x, float y )
      {
#ifdef WIN32
        return atan2f( x, y );
#else
        return __builtin_atan2f( x, y );
#endif
      }

      /*
       * Some additional functions
       */

      // returns true, if value is not a number
      static bool isNaN( float x )
      {
#ifdef WIN32
        return x != x;
#else
        return __builtin_isnanf( x );
#endif
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
