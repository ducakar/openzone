/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2014 Davorin Učakar
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
 * plain.vert
 *
 * Shader for UI.
 */

precision highp float;

uniform mat4 oz_ProjCamera;
uniform mat4 oz_Model;

attribute vec3 inPosition;
attribute vec2 inTexCoord;

varying vec2 exTexCoord;

void main()
{
  vec4 position = oz_Model * vec4(inPosition, 1.0);

  exTexCoord  = inTexCoord;
  gl_Position = oz_ProjCamera * position;
}
