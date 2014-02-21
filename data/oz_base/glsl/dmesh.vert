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
 * dmesh.vert
 *
 * Mesh shader that reads and interpolates vertex positions from the given vertex texture.
 */

#include "header.glsl"

#ifdef OZ_VERTEX_TEXTURE
uniform vec3 oz_MeshAnimation;
#endif

#include "attributes.glsl"
#include "varyings.glsl"

void main()
{
#ifdef OZ_VERTEX_TEXTURE

  float iVertex       = inPosition.x;
  float iPosition0    = oz_MeshAnimation[0] * 0.5;
  float iPosition1    = oz_MeshAnimation[1] * 0.5;
  float iNormal0      = 0.5 + oz_MeshAnimation[0] * 0.5;
  float iNormal1      = 0.5 + oz_MeshAnimation[1] * 0.5;
  float interpolation = oz_MeshAnimation[2];

  vec4  position0     = texture2D( oz_VertexAnim, vec2( iVertex, iPosition0 ) );
  vec4  position1     = texture2D( oz_VertexAnim, vec2( iVertex, iPosition1 ) );
  vec4  normal0       = texture2D( oz_VertexAnim, vec2( iVertex, iNormal0 ) );
  vec4  normal1       = texture2D( oz_VertexAnim, vec2( iVertex, iNormal1 ) );
  vec4  position      = oz_Model * mix( position0, position1, interpolation );
  vec3  normal        = normalize( mix( normal0, normal1, interpolation ).xyz );

#else

  vec4 position = oz_Model * vec4( inPosition, 1.0 );
  vec3 normal   = inNormal;

#endif

  gl_Position = oz_ProjCamera * position;
  exTexCoord  = inTexCoord;
  exNormal    = oz_ModelRot * normal;
#ifdef OZ_BUMP_MAP
  exTangent   = oz_ModelRot * inTangent;
  exBinormal  = oz_ModelRot * inBinormal;
#endif
  exLook      = position.xyz - oz_CameraPos;
}
