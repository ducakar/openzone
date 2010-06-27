/*
 *  Water.cc
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING file for details.
 */

#include "stable.hh"

#include "client/Water.hh"

#include "matrix/Timer.hh"
#include "client/Colours.hh"

namespace oz
{
namespace client
{

  Water water;

  const float Water::ALPHA = 0.75f;
  const float Water::TEX_BIAS = 0.5f;

  void Water::init()
  {
    phi = 0.0f;
  }

  void Water::update()
  {
    // update water
    phi = Math::mod( phi + timer.frameTime, 2.0f * Math::PI );;
    float ratio = ( 0.5f + Math::sin( 2.0f * phi ) / 2.0f );

    float alpha2 = ratio * ALPHA;
    float alpha1 = ( alpha2 * ( 1 - ratio ) ) / ( ratio * ( 1 - alpha2 ) );

    Colours::waterBlend1[3] = alpha1;
    Colours::waterBlend2[3] = alpha2;
  }

}
}
