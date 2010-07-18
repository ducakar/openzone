/*
 *  Math.hpp
 *
 *  Replacement for math.h and some other utility functions
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#ifdef OZ_GNU_MATH

# define fminf( x, y )          __builtin_fminf( x, y )
# define fmaxf( x, y )          __builtin_fmaxf( x, y )
# define fabsf( x )             __builtin_fabsf( x )
# define floorf( x )            __builtin_floorf( x )
# define ceilf( x )             __builtin_ceilf( x )
# define roundf( x )            __builtin_roundf( x )
# define fmodf( x, y )          __builtin_fmodf( x, y )
# define sqrtf( x )             __builtin_sqrtf( x )
# define expf( x )              __builtin_expf( x )
# define logf( x )              __builtin_logf( x )
# define powf( x, y )           __builtin_powf( x, y )
# define sinf( x )              __builtin_sinf( x )
# define cosf( x )              __builtin_cosf( x )
# define tanf( x )              __builtin_tanf( x )
# define asinf( x )             __builtin_asinf( x )
# define acosf( x )             __builtin_acosf( x )
# define atanf( x )             __builtin_atanf( x )
# define atan2f( x, y )         __builtin_atan2f( x, y )
# define nanf( x )              __builtin_nanf( x )
# define INFINITY               __builtin_inff()
# define isnanf( x )            __builtin_isnanf( x )
# define isinff( x )            __builtin_isinff( x )

# ifdef OZ_HAVE_SINCOSF
#  define sincosf( x, s, c )    __builtin_sincosf( x, s, c )
# else
#  define sincosf( x, s, c )    ( *( s ) = sinf( x ), *( c ) = cosf( x ) )
# endif

#else

# ifdef OZ_MINGW
#  include <math.h>
# else
#  include <cmath>
# endif

# define sincosf( x, s, c )     ( *( s ) = sinf( x ), *( c ) = cosf( x ) )
# define isnanf( x )            isnan( x )
# define isinff( x )            isinf( x )

# ifdef _MSC_VER

#  include <cfloat>

#  define fminf( x, y )         ( ( x ) < ( y ) ? ( x ) : ( y ) )
#  define fmaxf( x, y )         ( ( x ) > ( y ) ? ( x ) : ( y ) )
#  define roundf( x )           ( ( x ) < 0.0f ? ceilf( ( x ) - 0.5f ) : floorf( ( x ) + 0.5f ) )
#  define nanf( x )             float( 0.0f * HUGE_VAL )
#  define INFINITY              float( HUGE_VAL )
#  define isnan( x )            ( _isnan( x ) != 0 )
#  define isinf( x )            ( _isnan( x ) != 0 && !_finite( x ) != 0 )

# endif

#endif

namespace oz
{

  class Math
  {
    private:

      // static class
      Math() {}
      Math( const Math& ) = delete;
      Math& operator = ( const Math& ) = delete;

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

      static float min( float x, float y )
      {
        return fminf( x, y );
      }

      static float max( float x, float y )
      {
        return fmaxf( x, y );
      }

      static float bound( float x, float a, float b )
      {
        return fmaxf( a, fminf( x, b ) );
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

      static float mod( float x, float y )
      {
        return fmodf( x, y );
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
        sincosf( x, s, c );
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
        return isnanf( x );
      }

      static bool isInf( float x )
      {
        return isinff( x );
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

#ifdef OZ_GNU_MATH

# undef fminf
# undef fmaxf
# undef fabsf
# undef floorf
# undef ceilf
# undef roundf
# undef fmodf
# undef sqrtf
# undef expf
# undef logf
# undef powf
# undef sinf
# undef cosf
# undef sincosf
# undef tanf
# undef asinf
# undef acosf
# undef atanf
# undef atan2f
# undef nanf
# undef INFINITY
# undef isnanf
# undef isinff

#else

# undef sincosf
# undef isnanf
# undef isinff

# ifdef _MSC_VER

#  undef fminf
#  undef fmaxf
#  undef roundf
#  undef nanf
#  undef INFINITY
#  undef isnan
#  undef isinf

# endif

#endif
