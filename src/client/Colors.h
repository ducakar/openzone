/*
 *  Colors.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING for details.
 */

#pragma once

namespace oz
{
namespace client
{

  struct Colors
  {
    static const float BLACK[];
    static const float WHITE[];

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
