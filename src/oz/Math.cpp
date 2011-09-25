/*
 *  Math.cpp
 *
 *  Math constants
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "Math.hpp"

#include <cmath>
#include <cfloat>
#include <cstdlib>

namespace oz
{

  const float Math::EPSILON    = FLT_EPSILON;
  const float Math::E          = 2.718281828459045f;
  const float Math::TAU        = 6.283185307179586f;
  const float Math::INF        = float( INFINITY );
  const float Math::NaN        = float( NAN );

  void Math::seed( int n )
  {
    srand( uint( n ) );
  }

  int Math::rand( int max )
  {
    return ::rand() % max;
  }

  float Math::rand()
  {
    return float( ::rand() ) / float( RAND_MAX );
  }

}
