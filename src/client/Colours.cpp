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

  const Vec4 Colours::NONE  = Vec4( 0.0f, 0.0f, 0.0f, 0.0f );
  const Vec4 Colours::BLACK = Vec4( 0.0f, 0.0f, 0.0f, 1.0f );
  const Vec4 Colours::WHITE = Vec4( 1.0f, 1.0f, 1.0f, 1.0f );

  const Vec4 Colours::ENTITY_AABB = Vec4( 1.00f, 0.40f, 0.60f, 0.30f );
  const Vec4 Colours::STRUCTURE_AABB = Vec4( 0.20f, 0.50f, 1.00f, 0.30f );
  const Vec4 Colours::CLIP_AABB = Vec4( 0.60f, 0.90f, 0.20f, 0.30f );
  const Vec4 Colours::NOCLIP_AABB = Vec4( 0.70f, 0.80f, 0.90f, 0.30f );
  const Vec4 Colours::TAG = Vec4( 0.80f, 1.20f, 1.40f, 1.00f );

  const Vec4 Colours::GLOBAL_AMBIENT = Vec4( 0.20f, 0.20f, 0.20f, 1.00f );

  Vec4 Colours::diffuse = Vec4( 1.0f, 1.0f, 1.0f, 1.0f );
  Vec4 Colours::ambient = Vec4( 1.0f, 1.0f, 1.0f, 1.0f );
  Vec4 Colours::caelum = Vec4( 1.0f, 1.0f, 1.0f, 1.0f );
  Vec4 Colours::water = Vec4( 1.0f, 1.0f, 1.0f, 1.0f );

  Vec4 Colours::waterBlend1 = Vec4( 1.0f, 1.0f, 1.0f, 1.0f );
  Vec4 Colours::waterBlend2 = Vec4( 1.0f, 1.0f, 1.0f, 1.0f );

}
}
