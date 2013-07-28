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
 * mesh.frag
 *
 * Generic shader for meshes.
 */

#include "header.glsl"

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
  vec3  reflectDir   = reflect( exLook / dist, normal );
#endif

  vec4  colourSample = texture2D( oz_Textures[0], exTexCoord );
#ifndef OZ_LOW_DETAIL
  vec4  masksSample  = texture2D( oz_Textures[1], exTexCoord );
#endif

  vec3  base         = colourSample.xyz;
  vec3  ambient      = oz_CaelumLight.ambient;
  vec3  diffuse      = oz_CaelumLight.diffuse * max( 0.0, dot( oz_CaelumLight.dir, normal ) );
#ifdef OZ_LOW_DETAIL
  vec3  lighting     = min( ambient + diffuse, vec3( 1.25 ) );
#else
  float specularDot  = dot( oz_CaelumLight.dir, reflectDir );
  vec3  emission     = vec3( masksSample.g, masksSample.g, masksSample.g );
  vec3  specular     = oz_CaelumLight.diffuse * vec3( 2.0*masksSample.r * specularDot*specularDot );
  vec3  lighting     = min( ambient + diffuse + emission, vec3( 1.25 ) ) + specular;
# ifdef OZ_ENV_MAP
  vec3  environment  = textureCube( oz_Environment, reflectDir ).xyz;
  base               = mix( base, environment, masksSample.b );
# endif
#endif

  vec4  fragColour   = vec4( base * lighting, colourSample.w );

  gl_FragData[0]     = applyFog( oz_ColourTransform * fragColour, dist );
#ifdef OZ_POSTPROCESS
  gl_FragData[1]     = vec4( specular + emission, 1.0 );
#endif
}
