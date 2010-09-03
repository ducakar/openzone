/*
 *  Math.hpp
 *
 *  Replacement for math.h and some other utility functions
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include <cmath>

#ifdef OZ_MSVC

# include <cfloat>

# define fminf( x, y )          ( ( x ) < ( y ) ? ( x ) : ( y ) )
# define fmaxf( x, y )          ( ( x ) > ( y ) ? ( x ) : ( y ) )
# define roundf( x )            ( ( x ) < 0.0f ? ceilf( ( x ) - 0.5f ) : floorf( ( x ) + 0.5f ) )
# define truncf( x )            ( ( x ) < 0.0f ? ceilf( x ) : floorf( x ) )
# define nanf( x )              float( 0.0f * HUGE_VAL )
# define INFINITY               float( HUGE_VAL )

namespace std
{

  inline bool isnan( float x )
  {
    return _isnan( x ) != 0;
  }

  inline bool isfinite( float x )
  {
    return _finite( x ) != 0;
  }

  inline bool isinf( float x )
  {
    return _finite( x ) == 0 && _isnan( x ) == 0;
  }

  inline bool isnormal( float x )
  {
    return ( _fpclass( x ) & ( _FPCLASS_NN | _FPCLASS_PN ) ) != 0;
  }

}

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

      /*
       * Standard math functions
       */

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
        return fabsf( x );
      }

      static float floor( float x )
      {
        return floorf( x );
      }

      static float ceil( float x )
      {
        return ceilf( x );
      }

      static float round( float x )
      {
        return roundf( x );
      }

      static float trunc( float x )
      {
        return truncf( x );
      }

      static float mod( float x, float y )
      {
        return fmodf( x, y );
      }

      static Pair<float> fract( float x )
      {
        float integral;
        float fractional = modff( x, &integral );
        return Pair<float>( integral, fractional );
      }

      static float sqrt( float x )
      {
        return sqrtf( x );
      }

      static float exp( float x )
      {
        return expf( x );
      }

      static float log( float x )
      {
        return logf( x );
      }

      static float pow( float x, float y )
      {
        return powf( x, y );
      }

      static float sin( float x )
      {
        return sinf( x );
      }

      static float cos( float x )
      {
        return cosf( x );
      }

      static void sincos( float x, float* s, float* c )
      {
#ifdef OZ_HAVE_SINCOSF
        sincosf( x, s, c );
#else
        *s = sinf( x );
        *c = cosf( x );
#endif
      }

      static float tan( float x )
      {
        return tanf( x );
      }

      static float asin( float x )
      {
        return asinf( x );
      }

      static float acos( float x )
      {
        return acosf( x );
      }

      static float atan( float x )
      {
        return atanf( x );
      }

      static float atan2( float x, float y )
      {
        return atan2f( x, y );
      }

      /*
       * Some additional functions
       */

      static float nan()
      {
        return nanf( "" );
      }

      static float inf()
      {
        return INFINITY;
      }

      static bool isNaN( float x )
      {
        return std::isnan( x );
      }

      static bool isFinite( float x )
      {
        return std::isfinite( x );
      }

      static bool isInf( float x )
      {
        return std::isinf( x );
      }

      static bool isNormal( float x )
      {
        return std::isnormal( x );
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

      // fast inverse sqrt that appeared in Quake source (google for detailed explanations)
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

      // random integer between 0 and RAND_MAX == INT_MAX
      // (pointer to rand() function in cstdlib)
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

#ifdef OZ_MSVC

# undef fminf
# undef fmaxf
# undef roundf
# undef nanf
# undef INFINITY
# undef isnan
# undef isinf

#endif
