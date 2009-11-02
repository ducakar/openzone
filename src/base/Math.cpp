/*
 *  Math.cpp
 *
 *  Math constants
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#include "base.h"

#include <cmath>
#include <cstdlib>
#include <ctime>

namespace oz
{

  const Math Math::math;

  Math::Math()
  {
    srand( time( null ) );
  }

  void Math::seed( uint seed )
  {
    srand( seed );
  }

  int ( *const Math::rand )() = ::rand;

  float Math::frand()
  {
    return static_cast<float>( rand() ) / static_cast<float>( RAND_MAX );
  }

}
