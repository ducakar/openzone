/*
 *  bv.h
 *
 *  Bounding volumes
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3.0. See COPYING for details.
 */

#pragma once

namespace oz
{
  // ~1 mm, should be enough for 8 km x 8 km map (i guess)
  const float EPSILON = 0.0009765625;

}

#include "Bounds.h"
#include "AABB.h"
#include "Sphere.h"
