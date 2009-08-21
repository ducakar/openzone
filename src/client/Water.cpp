/*
 *  Water.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#include "precompiled.h"

#include "Water.h"

#include "matrix/Timer.h"

namespace oz
{
namespace client
{

  Water water;

  const float Water::ALPHA = 0.75f;
  const float Water::TEX_BIAS = 0.5f;

  void Water::update()
  {
    // update water
    phi = timer.millis / 500.0f;
    float ratio = ( 0.5f + Math::sin( phi ) / 2.0f );

    alpha2 = ratio * ALPHA;
    alpha1 = ( alpha2 * ( 1 - ratio ) ) / ( ratio * ( 1 - alpha2 ) );
  }

}
}
