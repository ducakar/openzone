/*
 *  Colours.cc
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING file for details.
 */

#include "stable.hh"

#include "client/Colours.hh"

namespace oz
{
namespace client
{

  const float Colours::BLACK[] = { 0.0f, 0.0f, 0.0f, 1.0f };
  const float Colours::WHITE[] = { 1.0f, 1.0f, 1.0f, 1.0f };

  const float Colours::STRUCTURE_AABB[] = { 0.00f, 0.00f, 1.00f, 0.30f };
  const float Colours::CLIP_AABB[] = { 1.00f, 1.00f, 0.00f, 0.30f };
  const float Colours::NOCLIP_AABB[] = { 0.80f, 0.80f, 1.00f, 0.30f };
  const float Colours::TAG[] = { -0.60f, 0.80f, 1.00f, 1.00f };

  const float Colours::GLOBAL_AMBIENT[] = { 0.20f, 0.20f, 0.20f, 1.00f };

  float Colours::diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
  float Colours::ambient[] = { 1.0f, 1.0f, 1.0f, 1.0f };
  float Colours::sky[] = { 1.0f, 1.0f, 1.0f, 1.0f };
  float Colours::water[] = { 1.0f, 1.0f, 1.0f, 1.0f };

  float Colours::waterBlend1[] = { 1.0f, 1.0f, 1.0f, 1.0f };
  float Colours::waterBlend2[] = { 1.0f, 1.0f, 1.0f, 1.0f };

}
}
