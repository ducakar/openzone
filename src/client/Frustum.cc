/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2019 Davorin Učakar
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

#include <client/Frustum.hh>

#include <client/Camera.hh>

namespace oz::client
{

Span Frustum::getExtremes(const Point& p) const
{
  return Span{
    max(int((p.x - radius_ + Orbis::DIM) / Cell::SIZE), 0),
    max(int((p.y - radius_ + Orbis::DIM) / Cell::SIZE), 0),
    min(int((p.x + radius_ + Orbis::DIM) / Cell::SIZE), Orbis::CELLS - 1),
    min(int((p.y + radius_ + Orbis::DIM) / Cell::SIZE), Orbis::CELLS - 1)
  };
}

void Frustum::update()
{
  float fovX = Math::atan(camera.coeff * camera.mag * camera.aspect);
  float fovY = Math::atan(camera.coeff * camera.mag);

  float sx = 0.0f, cx = 0.0f, sy = 0.0f, cy = 0.0f;
  Math::sincos(fovX, &sx, &cx);
  Math::sincos(fovY, &sy, &cy);

  Vec3 nLeft  = camera.rotMat * Vec3(  cx, 0.0f, -sx);
  Vec3 nRight = camera.rotMat * Vec3( -cx, 0.0f, -sx);
  Vec3 nDown  = camera.rotMat * Vec3(0.0f,   cy, -sy);
  Vec3 nUp    = camera.rotMat * Vec3(0.0f,  -cy, -sy);
  Vec3 nFront = camera.at;

  float dLeft  = camera.p * nLeft;
  float dRight = camera.p * nRight;
  float dDown  = camera.p * nDown;
  float dUp    = camera.p * nUp;
  float dFront = camera.p * nFront + camera.maxDist;

  left_  = Plane(nLeft,  dLeft );
  right_ = Plane(nRight, dRight);
  down_  = Plane(nDown,  dDown );
  up_    = Plane(nUp,    dUp   );
  front_ = Plane(nFront, dFront);

  radius_ = camera.maxDist / cx;
}

Frustum frustum;

}
