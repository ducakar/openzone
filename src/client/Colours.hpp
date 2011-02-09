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
    static const Quat BLACK;
    static const Quat WHITE;

    static const Quat ENTITY_AABB;
    static const Quat STRUCTURE_AABB;
    static const Quat CLIP_AABB;
    static const Quat NOCLIP_AABB;
    static const Quat TAG;

    static const Quat GLOBAL_AMBIENT;

    static Quat diffuse;
    static Quat ambient;
    static Quat sky;
    static Quat water;

    static Quat waterBlend1;
    static Quat waterBlend2;
  };

}
}
