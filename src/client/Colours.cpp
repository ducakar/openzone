/*
 *  Colours.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "client/Colours.hpp"

namespace oz
{
namespace client
{

  const Quat Colours::NONE  = Quat( 0.0f, 0.0f, 0.0f, 0.0f );
  const Quat Colours::BLACK = Quat( 0.0f, 0.0f, 0.0f, 1.0f );
  const Quat Colours::WHITE = Quat( 1.0f, 1.0f, 1.0f, 1.0f );

  const Quat Colours::ENTITY_AABB = Quat( 1.00f, 0.20f, 0.50f, 0.30f );
  const Quat Colours::STRUCTURE_AABB = Quat( 0.20f, 0.50f, 1.00f, 0.30f );
  const Quat Colours::CLIP_AABB = Quat( 0.50f, 1.00f, 0.20f, 0.30f );
  const Quat Colours::NOCLIP_AABB = Quat( 0.70f, 0.80f, 0.90f, 0.30f );

  const Quat Colours::GLOBAL_AMBIENT = Quat( 0.20f, 0.20f, 0.20f, 1.00f );

  Quat Colours::diffuse = Quat( 1.0f, 1.0f, 1.0f, 1.0f );
  Quat Colours::ambient = Quat( 1.0f, 1.0f, 1.0f, 1.0f );
  Quat Colours::sky = Quat( 1.0f, 1.0f, 1.0f, 1.0f );
  Quat Colours::water = Quat( 1.0f, 1.0f, 1.0f, 1.0f );

  Quat Colours::waterBlend1 = Quat( 1.0f, 1.0f, 1.0f, 1.0f );
  Quat Colours::waterBlend2 = Quat( 1.0f, 1.0f, 1.0f, 1.0f );

}
}
