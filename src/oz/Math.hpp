/*
 *  Math.hpp
 *
 *  Replacement for math.h and some other utility functions
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#ifdef OZ_MSVC
# include <cfloat>
#endif

namespace oz
{

  class Math
  {
    private:

      // static class
      explicit Math() {}
      Math( const Math& );
      Math& operator = ( const Math& );

    public:

      static const float EPSILON;
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

      static const float MAX_RAND;

      static float min( float x, float y )
      {
        return y < x ? y : x;
      }

      static float max( float x, float y )
      {
        return x < y ? y : x;
      }

      static float bound( float x, float a, float b )
      {
        return x < a ? a : ( b < x ? b : x );
      }

      static float abs( float x )
      {
#ifdef OZ_BUILTIN_MATH
        return __builtin_fabsf( x );
#else
        return fabsf( x );
#endif
      }

      static float floor( float x )
      {
#ifdef OZ_BUILTIN_MATH
        return __builtin_floorf( x );
#else
        return floorf( x );
#endif
      }

      static float ceil( float x )
      {
#ifdef OZ_BUILTIN_MATH
        return __builtin_ceilf( x );
#else
        return ceilf( x );
#endif
      }

      static float round( float x )
      {
#ifdef OZ_BUILTIN_MATH
        return __builtin_roundf( x );
#elif defined( OZ_MSVC )
        return x < 0.0f ? ceilf( x - 0.5f ) : floorf( x + 0.5f );
#else
        return roundf( x );
#endif
      }

      static float trunc( float x )
      {
#ifdef OZ_BUILTIN_MATH
        return __builtin_truncf( x );
#elif defined( OZ_MSVC )
        return x < 0.0f ? ceilf( x ) : floorf( x );
#else
        return truncf( x );
#endif
      }

      static float mod( float x, float y )
      {
#ifdef OZ_BUILTIN_MATH
        return __builtin_fmodf( x, y );
#else
        return fmodf( x, y );
#endif
      }

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

      static float sqrt( float x )
      {
#ifdef OZ_BUILTIN_MATH
        return __builtin_sqrtf( x );
#else
        return sqrtf( x );
#endif
      }

      static float exp( float x )
      {
#ifdef OZ_BUILTIN_MATH
        return __builtin_expf( x );
#else
        return expf( x );
#endif
      }

      static float log( float x )
      {
#ifdef OZ_BUILTIN_MATH
        return __builtin_logf( x );
#else
        return logf( x );
#endif
      }

      static float pow( float x, float y )
      {
#ifdef OZ_BUILTIN_MATH
        return __builtin_powf( x, y );
#else
        return powf( x, y );
#endif
      }

      static float sin( float x )
      {
#ifdef OZ_BUILTIN_MATH
        return __builtin_sinf( x );
#else
        return sinf( x );
#endif
      }

      static float cos( float x )
      {
#ifdef OZ_BUILTIN_MATH
        return __builtin_cosf( x );
#else
        return cosf( x );
#endif
      }

      static void sincos( float x, float* s, float* c )
      {
#ifdef OZ_HAVE_SINCOSF
# ifdef OZ_BUILTIN_MATH
        __builtin_sincosf( x, s, c );
# else
        sincosf( x, s, c );
# endif
#else
# ifdef OZ_BUILTIN_MATH
        *s = __builtin_sinf( x );
        *c = __builtin_cosf( x );
# else
        *s = sinf( x );
        *c = cosf( x );
# endif
#endif
      }

      static float tan( float x )
      {
#ifdef OZ_BUILTIN_MATH
        return __builtin_tanf( x );
#else
        return tanf( x );
#endif
      }

      static float asin( float x )
      {
#ifdef OZ_BUILTIN_MATH
        return __builtin_asinf( x );
#else
        return asinf( x );
#endif
      }

      static float acos( float x )
      {
#ifdef OZ_BUILTIN_MATH
        return __builtin_acosf( x );
#else
        return acosf( x );
#endif
      }

      static float atan( float x )
      {
#ifdef OZ_BUILTIN_MATH
        return __builtin_atanf( x );
#else
        return atanf( x );
#endif
      }

      static float atan2( float x, float y )
      {
#ifdef OZ_BUILTIN_MATH
        return __builtin_atan2f( x, y );
#else
        return atan2f( x, y );
#endif
      }

      static float nan()
      {
#ifdef OZ_BUILTIN_MATH
        return __builtin_nanf( "" );
#elif defined( OZ_MSVC )
        return float( 0.0f * HUGE_VAL );
#else
        return nanf( "" );
#endif
      }

      static float inf()
      {
#ifdef OZ_BUILTIN_MATH
        return __builtin_inff();
#else
        return float( HUGE_VAL );
#endif
      }

      static bool isNaN( float x )
      {
#ifdef OZ_BUILTIN_MATH
        return __builtin_isnan( x );
#elif defined( OZ_MSVC )
        return _isnan( x ) != 0;
#else
        return std::isnan( x );
#endif
      }

      static bool isFinite( float x )
      {
#ifdef OZ_BUILTIN_MATH
        return __builtin_isfinite( x );
#elif defined( OZ_MSVC )
        return _finite( x ) != 0;
#else
        return std::isfinite( x );
#endif
      }

      static bool isInf( float x )
      {
#ifdef OZ_BUILTIN_MATH
        return __builtin_isinf( x );
#elif defined( OZ_MSVC )
        return _finite( x ) == 0 && _isnan( x ) == 0;
#else
        return std::isinf( x );
#endif
      }

      static bool isNormal( float x )
      {
#ifdef OZ_BUILTIN_MATH
        return __builtin_isnormal( x );
#elif defined( OZ_MSVC )
        return ( _fpclass( x ) & ( _FPCLASS_NN | _FPCLASS_PN ) ) != 0;
#else
        return std::isnormal( x );
#endif
      }

      static float sgn( float x )
      {
        return x < 0.0f ? -1.0f : ( x > 0.0f ? 1.0f : 0.0f );
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
        union FloatToBits
        {
          float f;
          int   b;
        };
        FloatToBits fb = { x };
        return fb.b;
      }

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

      static float fastSqrt( float x )
      {
         return x * fastInvSqrt( x );
      }

      // famous fast inverse sqrt from Quake source (google for detailed explanations)
      static float fastInvSqrt( float x )
      {
        float y = fromBits( 0x5f3759df - ( toBits( x ) >> 1 ) );
        return y * ( 1.5f - 0.5f * x * y*y );
      }

      // is power of two?
      template <typename Value>
      static bool isPow2( const Value& v )
      {
        return ( v & ( v - 1 ) ) == 0;
      }

      // pointer to srand in cstdlib
      static void ( *const seed )( uint seed );

      // random integer between 0 and RAND_MAX
      // (pointer to rand() function in <cstdlib>)
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
