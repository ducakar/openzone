/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2013 Davorin Učakar
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

/*
 * terraLiquid.vert
 *
 * Terrain (lava) sea surface shader.
 */

#include "header.glsl"

const float TERRA_WATER_SCALE = 512.0;
const vec3  NORMAL            = vec3( 0.0, 0.0, 1.0 );

uniform float oz_WaveBias;

#include "attributes.glsl"
#include "varyings.glsl"

float noise( vec2 pos, float t )
{
  return sin( pos.x*pos.x + pos.y*pos.y + t );
}

void main()
{
  float z        = 0.15 * sin( oz_WaveBias + inPosition.x + inPosition.y );
  vec4  position = oz_Model * vec4( inPosition.x, inPosition.y, z, 1.0 );
  float dx       = 0.2 * noise( 0.1 * position.xy, oz_WaveBias );
  float dy       = 0.2 * noise( 0.1 * position.yx, oz_WaveBias );

  gl_Position = oz_ProjCamera * position;
  exTexCoord  = inTexCoord * TERRA_WATER_SCALE;
  exNormal    = vec3( dx, dy, 1.0 );
#ifdef OZ_BUMP_MAP
  exTangent   = vec3( 1.0, 0.0, 0.0 );
  exBinormal  = vec3( 0.0, 1.0, 0.0 );
#endif
  exLook      = position.xyz - oz_CameraPos;
}
