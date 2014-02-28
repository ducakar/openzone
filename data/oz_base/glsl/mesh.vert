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
 * mesh.vert
 *
 * Generic shader for meshes.
 */

#include "header.glsl"

attribute vec3 inPosition;
attribute vec2 inTexCoord;
attribute vec3 inNormal;
#ifdef OZ_BUMP_MAP
attribute vec3 inTangent;
attribute vec3 inBinormal;
#endif

varying vec2 exTexCoord;
varying vec3 exNormal;
#ifdef OZ_BUMP_MAP
varying vec3 exTangent;
varying vec3 exBinormal;
#endif
varying vec3 exLook;

void main()
{
  vec4 position = oz_Model * vec4( inPosition, 1.0 );

  gl_Position = oz_ProjCamera * position;
  exTexCoord  = inTexCoord;
  exNormal    = oz_ModelRot * inNormal;
#ifdef OZ_BUMP_MAP
  exTangent   = oz_ModelRot * inTangent;
  exBinormal  = oz_ModelRot * inBinormal;
#endif
  exLook      = position.xyz - oz_CameraPos;
}
