/*
 *  Colours.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
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
    static const Vec4 NONE;
    static const Vec4 BLACK;
    static const Vec4 WHITE;

    static const Vec4 ENTITY_AABB;
    static const Vec4 STRUCTURE_AABB;
    static const Vec4 CLIP_AABB;
    static const Vec4 NOCLIP_AABB;
    static const Vec4 TAG;

    static const Vec4 GLOBAL_AMBIENT;

    static Vec4 diffuse;
    static Vec4 ambient;
    static Vec4 caelum;
    static Vec4 water;

    static Vec4 waterBlend1;
    static Vec4 waterBlend2;
  };

}
}
