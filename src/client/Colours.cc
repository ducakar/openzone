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
 * @file client/Colours.cc
 */

#include "stable.hh"

#include "client/Colours.hh"

namespace oz
{
namespace client
{

const Vec4 Colours::NONE  = Vec4( 0.0f, 0.0f, 0.0f, 0.0f );
const Vec4 Colours::BLACK = Vec4( 0.0f, 0.0f, 0.0f, 1.0f );
const Vec4 Colours::WHITE = Vec4( 1.0f, 1.0f, 1.0f, 1.0f );

const Vec4 Colours::ENTITY_AABB = Vec4( 1.00f, 0.40f, 0.60f, 0.30f );
const Vec4 Colours::STRUCTURE_AABB = Vec4( 0.20f, 0.50f, 1.00f, 0.30f );
const Vec4 Colours::CLIP_AABB = Vec4( 0.60f, 0.90f, 0.20f, 0.30f );
const Vec4 Colours::NOCLIP_AABB = Vec4( 0.70f, 0.80f, 0.90f, 0.30f );

const Vec4 Colours::GLOBAL_AMBIENT = Vec4( 0.08f, 0.08f, 0.08f, 1.00f );

Vec4 Colours::diffuse = Vec4( 1.0f, 1.0f, 1.0f, 1.0f );
Vec4 Colours::ambient = Vec4( 1.0f, 1.0f, 1.0f, 1.0f );
Vec4 Colours::caelum = Vec4( 1.0f, 1.0f, 1.0f, 1.0f );
Vec4 Colours::water = Vec4( 1.0f, 1.0f, 1.0f, 1.0f );

Vec4 Colours::waterBlend1 = Vec4( 1.0f, 1.0f, 1.0f, 1.0f );
Vec4 Colours::waterBlend2 = Vec4( 1.0f, 1.0f, 1.0f, 1.0f );

}
}
