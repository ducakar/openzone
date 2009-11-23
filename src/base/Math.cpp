/*
 *  Math.cpp
 *
 *  Math constants
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3.0. See COPYING for details.
 */

#include "base.h"

#include <cmath>
#include <cstdlib>
#include <ctime>

namespace oz
{

  const Math Math::math;

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
