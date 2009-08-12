/*
 *  bv.h
 *
 *  Bounding volumes
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

namespace oz
{
  // ~1 mm, should be enough for 8 km x 8 km map (experimentally)
  extern const float EPSILON;

}

#include "Bounds.h"
#include "AABB.h"
#include "Sphere.h"
