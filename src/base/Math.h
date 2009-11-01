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

      static const float FLOAT_EPS  = 1.1920928955078125e-7f;
      static const float E          = 2.7182818284590452354f;
      static const float LOG2E      = 1.4426950408889634074f;
      static const float LOG10E     = 0.43429448190325182765f;
      static const float LN2        = 0.69314718055994530942f;
      static const float LN10       = 2.30258509299404568402f;
      static const float PI         = 3.14159265358979323846f;
      static const float PI_2       = 1.57079632679489661923f;
      static const float PI_4       = 0.78539816339744830962f;
      static const float _1_PI      = 0.31830988618379067154f;
      static const float _2_PI      = 0.63661977236758134308f;
      static const float _2_SQRTPI  = 1.12837916709551257390f;
      static const float SQRT2      = 1.41421356237309504880f;
      static const float SQRT1_2    = 0.70710678118654752440f;

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
#ifdef __WIN32__
        *s = __builtin_sinf( x );
        *c = __builtin_cosf( x );
#else
        __builtin_sincosf( x, s, c );
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
        return __builtin_isinf( x );
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

      // fast inverse sqrt that appeared in Quake source (google for detailed explanations)
      static float fInvSqrt( float x )
      {
        float y = fromBits( 0x5f3759df - ( toBits( x ) >> 1 ) );
        return y * ( 1.5f - 0.5f * x * y*y );
      }

      static const int &toBits( const float &f )
      {
        return *reinterpret_cast<const int*>( &f );
      }

      static const float &fromBits( const int &b )
      {
        return *reinterpret_cast<const float*>( &b );
      }

      // is power of two?
      template <class Value>
      static bool isPow2( const Value &v )
      {
        return v & ( v - 1 ) == 0;
      }

      static void seed( uint seed );

      // random integer from 0 to RAND_MAX == INT_MAX
      // (pointer to rand() function in stdlib.h)
      static int ( *const rand )();

      // random float from interval [0, 1]
      static float frand()
      {
        return rand() / static_cast<float>( INT_MAX );
      }

  };

}
