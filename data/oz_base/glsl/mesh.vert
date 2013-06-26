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
 * mesh.vert
 *
 * Generic shader for meshes.
 */

#include "header.glsl"

attribute vec3 inPosition;
attribute vec2 inTexCoord;
attribute vec3 inNormal;

varying vec3 exPosition;
varying vec2 exTexCoord;
varying vec3 exNormal;

void main()
{
  gl_Position = oz_ProjModelTransform * vec4( inPosition, 1.0 );
  exPosition  = ( oz_ModelTransform * vec4( inPosition, 1.0 ) ).xyz;
  exTexCoord  = inTexCoord;
  exNormal    = ( oz_ModelTransform * vec4( inNormal, 0.0 ) ).xyz;
}
