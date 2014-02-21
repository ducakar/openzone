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
 * plant.vert
 *
 * Mesh shader that deforms mesh according to the given wind.
 */

#include "header.glsl"

uniform vec4 oz_Wind;

#include "attributes.glsl"
#include "varyings.glsl"

void main()
{
  vec4  position = oz_Model * vec4( inPosition, 1.0 );
  float windFact = max( inPosition.z, 0.0 );
  vec2  windBias = oz_Wind.xy * windFact*windFact * oz_Wind.z *
                     sin( 0.08 * ( position.x + position.y ) + oz_Wind.w );

  position    = vec4( position.xy + windBias.xy, position.z, 1.0 );

  gl_Position = oz_ProjCamera * position;
  exTexCoord  = inTexCoord;
  exNormal    = oz_ModelRot * inNormal;
#ifdef OZ_BUMP_MAP
  exTangent   = oz_ModelRot * inTangent;
  exBinormal  = oz_ModelRot * inBinormal;
#endif
  exLook      = position.xyz - oz_CameraPos;
}
