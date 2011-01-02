/*
 *  Colours.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

namespace oz
{
namespace client
{

  struct Colours
  {
    static const float BLACK[];
    static const float WHITE[];

    static const float STRUCTURE_AABB[];
    static const float CLIP_AABB[];
    static const float NOCLIP_AABB[];
    static const float TAG[];

    static const float GLOBAL_AMBIENT[];

    static float diffuse[];
    static float ambient[];
    static float sky[];
    static float water[];

    static float waterBlend1[];
    static float waterBlend2[];
  };

}
}
