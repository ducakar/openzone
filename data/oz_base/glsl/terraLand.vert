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
 * terraLand.vert
 *
 * Terrain (land) shader.
 */

precision highp float;

uniform mat4  oz_ProjCamera;
uniform mat4  oz_Model;
uniform vec3  oz_CameraPos;

attribute vec3 inPosition;
attribute vec2 inTexCoord;
attribute vec3 inNormal;
#ifdef OZ_BUMP_MAP
attribute vec3 inTangent;
attribute vec3 inBinormal;
#endif

varying vec3  exPosition;
varying vec2  exTexCoord;
varying vec3  exNormal;
#ifdef OZ_BUMP_MAP
varying vec3  exTangent;
varying vec3  exBinormal;
#endif

void main()
{
  mat3 modelRot = mat3(oz_Model);
  vec4 position = oz_Model * vec4(inPosition, 1.0);

  exPosition  = position.xyz - oz_CameraPos;
  exTexCoord  = inTexCoord;
  exNormal    = modelRot * inNormal;
#ifdef OZ_BUMP_MAP
  exTangent   = modelRot * inTangent;
  exBinormal  = modelRot * inBinormal;
#endif
  gl_Position = oz_ProjCamera * position;
}
