/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2011 Davorin Učakar
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
 * @file client/Colours.hh
 */

#pragma once

#include "client/common.hh"

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