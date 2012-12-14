/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2012 Davorin Učakar
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
 * terraWater.vert
 *
 * Terrain sea surface shader.
 */

const float TERRA_WATER_SCALE = 512.0;
const vec3  NORMAL            = vec3( 0.0, 0.0, 1.0 );

uniform float oz_WaveBias;

attribute vec3 inPosition;
attribute vec2 inTexCoord;
attribute vec3 inNormal;

varying vec3 exPosition;
varying vec2 exTexCoord;
varying vec3 exNormal;

void main()
{
  float z        = 0.15 * sin( oz_WaveBias + inPosition.x + inPosition.y );
  vec4  localPos = vec4( inPosition.x, inPosition.y, z, 1.0 );
  vec3  position = ( oz_ModelTransform * localPos ).xyz;

  gl_Position    = oz_ProjModelTransform * localPos;
  exPosition     = position;
  exTexCoord     = inTexCoord * TERRA_WATER_SCALE;
  exNormal       = NORMAL;
}
