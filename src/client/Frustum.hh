/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2016 Davorin Učakar
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @file client/Frustum.hh
 */

#pragma once

#include <client/common.hh>

namespace oz::client
{

class Frustum
{
private:

  Plane left_;
  Plane right_;
  Plane down_;
  Plane up_;
  Plane front_;
  float radius_;

public:

  OZ_ALWAYS_INLINE
  float radius() const
  {
    return radius_;
  }

  OZ_ALWAYS_INLINE
  bool isVisible(const Point& p, float radius = 0.0f) const
  {
    return p * left_  > -radius &&
           p * right_ > -radius &&
           p * up_    > -radius &&
           p * down_  > -radius &&
           p * front_ < +radius;
  }

  OZ_ALWAYS_INLINE
  bool isVisible(float x, float y, float radius) const
  {
    Point mins = Point(x, y, -Orbis::DIM);
    Point maxs = Point(x, y, +Orbis::DIM);

    return (mins * left_  > -radius || maxs * left_  > -radius) &&
           (mins * right_ > -radius || maxs * right_ > -radius) &&
           (mins * up_    > -radius || maxs * up_    > -radius) &&
           (mins * down_  > -radius || maxs * down_  > -radius) &&
           (mins * front_ < +radius || maxs * front_ < +radius);
  }

  // get min and max index for cells per each axis, which should be included in PVS
  Span getExtremes(const Point& p) const;

  void update();

};

extern Frustum frustum;

}
