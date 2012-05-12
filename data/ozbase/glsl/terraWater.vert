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

const lowp float TERRA_WATER_SCALE = 512.0;
const lowp vec3  NORMAL            = vec3( 0.0, 0.0, 1.0 );

uniform lowp float oz_WaveBias;

attribute lowp vec3 inPosition;
attribute lowp vec2 inTexCoord;
attribute lowp vec3 inNormal;

varying lowp vec3 exPosition;
varying lowp vec2 exTexCoord;
varying lowp vec3 exNormal;

void main()
{
  lowp float z        = 0.15 * sin( oz_WaveBias + inPosition.x + inPosition.y );
  lowp vec4  localPos = vec4( inPosition.x, inPosition.y, z, 1.0 );
  lowp vec3  position = ( oz_Transform.model * localPos ).xyz;

  gl_Position = oz_Transform.complete * localPos;
  exPosition  = position;
  exTexCoord  = inTexCoord * TERRA_WATER_SCALE;
  exNormal    = NORMAL;
}
