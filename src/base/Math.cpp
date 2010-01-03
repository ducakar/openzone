/*
 *  Math.cpp
 *
 *  Math constants
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING for details.
 */

#include "base.h"

#include <cmath>
#include <cstdlib>
#include <ctime>

namespace oz
{

  const float Math::MAX_RAND = float( RAND_MAX );

  void ( *const Math::seed )( uint ) = ::srand;
  int ( *const Math::rand )() = ::rand;

  float Math::frand()
  {
    return float( rand() ) / MAX_RAND;
  }

}
