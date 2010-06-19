/*
 *  Colors.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING for details.
 */

#include "stable.hpp"

#include "client/Colors.hpp"

namespace oz
{
namespace client
{

  const float Colors::BLACK[] = { 0.0f, 0.0f, 0.0f, 1.0f };
  const float Colors::WHITE[] = { 1.0f, 1.0f, 1.0f, 1.0f };

  const float Colors::CLIP_AABB[] = { 1.00f, 1.00f, 0.00f, 0.30f };
  const float Colors::NOCLIP_AABB[] = { 0.80f, 0.80f, 1.00f, 0.30f };
  const float Colors::TAG[] = { -0.60f, 0.80f, 1.00f, 1.00f };

  const float Colors::GLOBAL_AMBIENT[] = { 0.20f, 0.20f, 0.20f, 1.00f };

  float Colors::diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
  float Colors::ambient[] = { 1.0f, 1.0f, 1.0f, 1.0f };
  float Colors::sky[] = { 1.0f, 1.0f, 1.0f, 1.0f };
  float Colors::water[] = { 1.0f, 1.0f, 1.0f, 1.0f };

  float Colors::waterBlend1[] = { 1.0f, 1.0f, 1.0f, 1.0f };
  float Colors::waterBlend2[] = { 1.0f, 1.0f, 1.0f, 1.0f };

}
}
