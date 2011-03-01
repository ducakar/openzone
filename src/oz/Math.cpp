/*
 *  Math.cpp
 *
 *  Math constants
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "Math.hpp"

#include <cmath>
#include <cfloat>
#include <cstdlib>
#include <ctime>

namespace oz
{

  const float Math::EPSILON    = FLT_EPSILON;
  const float Math::E          = 2.718281828459045f;
  const float Math::TAU        = 6.283185307179586f;

#ifdef OZ_MSVC
  const float Math::INF        = float( HUGE_VAL );
  const float Math::NaN        = float( 0.0 * HUGE_VAL );
#else
  const float Math::INF        = float( INFINITY );
  const float Math::NaN        = float( NAN );
#endif

  const float Math::MAX_RAND = float( RAND_MAX );

  void ( *const Math::seed )( uint ) = ::srand;
  int ( *const Math::rand )() = ::rand;

  float Math::frand()
  {
    return float( rand() ) / MAX_RAND;
  }

}
