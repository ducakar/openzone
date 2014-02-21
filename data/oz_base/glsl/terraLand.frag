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
 * terraLand.frag
 *
 * Terrain (land) shader.
 */

#include "header.glsl"

const float TERRA_DETAIL_SCALE = 768.0;

varying vec2 exTexCoord;
varying vec3 exNormal;
varying vec3 exLook;

void main()
{
  vec3  normal       = normalize( exNormal );
#ifdef OZ_LOW_DETAIL
  float dist         = 1.0 / gl_FragCoord.w;
#else
  float dist         = length( exLook );
#endif

  vec4  detailSample = texture2D( oz_Textures[0], exTexCoord * TERRA_DETAIL_SCALE );
  vec4  mapSample    = texture2D( oz_Textures[1], exTexCoord );
  vec3  ambient      = oz_CaelumLight.ambient;
  vec3  diffuse      = oz_CaelumLight.diffuse * max( 0.0, dot( oz_CaelumLight.dir, normal ) );
  vec3  lighting     = min( ambient + diffuse, vec3( 1.25 ) );
  vec4  fragColour   = vec4( detailSample.xyz * mapSample.xyz * lighting, 1.0 );

  gl_FragData[0]     = applyFog( oz_Colour * fragColour, dist );
#ifdef OZ_POSTPROCESS
  gl_FragData[1]     = vec4( 0.0, 0.0, 0.0, 1.0 );
#endif
}
