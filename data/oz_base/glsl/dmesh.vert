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
 * dmesh.vert
 *
 * Mesh shader that reads and interpolates vertex positions from the given vertex texture.
 */

#include "header.glsl"

#ifdef OZ_VERTEX_TEXTURE
uniform vec3 oz_MeshAnimation;
#endif

attribute vec3 inPosition;
attribute vec2 inTexCoord;
attribute vec3 inNormal;

varying vec3 exPosition;
varying vec2 exTexCoord;
varying vec3 exNormal;

void main()
{
#ifdef OZ_VERTEX_TEXTURE

  vec4 firstPosition  = texture2D( oz_Textures[2], vec2( inPosition.x, oz_MeshAnimation[0] ) );
  vec4 secondPosition = texture2D( oz_Textures[2], vec2( inPosition.x, oz_MeshAnimation[1] ) );
  vec4 firstNormal    = texture2D( oz_Textures[3], vec2( inPosition.x, oz_MeshAnimation[0] ) );
  vec4 secondNormal   = texture2D( oz_Textures[3], vec2( inPosition.x, oz_MeshAnimation[1] ) );
  vec4 localPosition  = vec4( mix( firstPosition, secondPosition, oz_MeshAnimation[2] ).xyz, 1.0 );
  vec4 localNormal    = vec4( mix( firstNormal, secondNormal, oz_MeshAnimation[2] ).xyz, 0.0 );

  gl_Position   = oz_ProjModelTransform * localPosition;
  exPosition    = ( oz_ModelTransform * localPosition ).xyz;
  exTexCoord    = inTexCoord;
  exNormal      = ( oz_ModelTransform * localNormal ).xyz;

#else

  gl_Position = oz_ProjModelTransform * vec4( inPosition, 1.0 );
  exPosition  = ( oz_ModelTransform * vec4( inPosition, 1.0 ) ).xyz;
  exTexCoord  = inTexCoord;
  exNormal    = ( oz_ModelTransform * vec4( inNormal, 0.0 ) ).xyz;

#endif
}
