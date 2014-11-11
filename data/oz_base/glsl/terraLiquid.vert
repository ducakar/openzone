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
 * terraLiquid.vert
 *
 * Terrain (lava) sea surface shader.
 */

precision highp float;

const float TERRA_WATER_SCALE = 512.0;

uniform mat4  oz_ProjCamera;
uniform mat4  oz_Model;
uniform vec3  oz_CameraPos;
uniform float oz_WaveBias;

attribute vec3 inPosition;
attribute vec2 inTexCoord;
attribute vec3 inNormal;
#ifdef OZ_BUMP_MAP
attribute vec3 inTangent;
attribute vec3 inBinormal;
#endif

varying vec3 exPosition;
varying vec2 exTexCoord;
varying vec3 exNormal;
#ifdef OZ_BUMP_MAP
varying vec3 exTangent;
varying vec3 exBinormal;
#endif

#ifdef OZ_VERTEX_EFFECTS
float noise(vec2 pos, float seed)
{
  return fract(sin(dot(pos, vec2(431.0, 683.0))) * seed) * 3.141592653589793;
}
#endif

void main()
{
#ifdef OZ_VERTEX_EFFECTS
  vec4  position = oz_Model * vec4(inPosition.xy, 0.0, 1.0);
  float alpha    = oz_WaveBias + noise(position.xy, 43758.5453);
  float beta     = oz_WaveBias + noise(position.xy, 76283.5127);
  float sinAlpha = sin(alpha);
  float cosAlpha = 1.0 - 0.5 * sinAlpha*sinAlpha;
  float dx       = 0.25 * cosAlpha * cos(beta);
  float dy       = 0.25 * cosAlpha * sin(beta);

  position.z     = 0.15 * sinAlpha;
#else
  mat3  modelRot = mat3(oz_Model);
  vec4  position = oz_Model * vec4(inPosition.x, inPosition.y, 0.0, 1.0);
#endif

  exPosition  = position.xyz - oz_CameraPos;
  exTexCoord  = inTexCoord;
#ifdef OZ_VERTEX_EFFECTS
  exNormal    = vec3(dx, dy, 1.0);
#else
  exNormal    = vec3(0.0, 0.0, 1.0);
#endif
#ifdef OZ_BUMP_MAP
  exTangent   = vec3(1.0, 0.0, 0.0);
  exBinormal  = vec3(0.0, 1.0, 0.0);
#endif
  gl_Position = oz_ProjCamera * position;
}
