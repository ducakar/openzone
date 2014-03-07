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

#version 100

precision mediump float;

struct CaelumLight
{
  vec3 dir;
  vec3 diffuse;
  vec3 ambient;
};

struct Fog
{
  vec4  colour;
  float dist2;
};

uniform mat4        oz_Colour;
uniform sampler2D   oz_Texture;
uniform sampler2D   oz_Masks;
uniform sampler2D   oz_Normals;
uniform samplerCube oz_EnvMap;
uniform CaelumLight oz_CaelumLight;
uniform Fog         oz_Fog;

varying vec2 exTexCoord;
varying vec3 exNormal;
#ifdef OZ_BUMP_MAP
varying vec3 exTangent;
varying vec3 exBinormal;
#endif
varying vec3 exLook;

vec3 pixelNormal( sampler2D texture, vec2 texCoord )
{
  vec3 texel = texture2D( texture, texCoord ).xyz;
  return 2.0 * texel - vec3( 1.0 );
}

vec4 applyFog( vec4 colour, float dist )
{
  float ratio = min( dist*dist / oz_Fog.dist2, 1.0 );
  return mix( colour, oz_Fog.colour, ratio*ratio );
}

void main()
{
  vec3  normal       = normalize( exNormal );
#ifdef OZ_BUMP_MAP
  vec3  tangent      = normalize( exTangent );
  vec3  binormal     = normalize( exBinormal );
  mat3  plane        = mat3( tangent, binormal, normal );

  normal             = plane * pixelNormal( oz_Normals, exTexCoord );
#endif
#ifdef OZ_LOW_DETAIL
  float dist         = 1.0 / gl_FragCoord.w;
#else
  float dist         = length( exLook );
  vec3  reflectDir   = reflect( exLook / dist, normal );
#endif

  vec4  colourSample = texture2D( oz_Texture, exTexCoord );
#ifndef OZ_LOW_DETAIL
  vec4  masksSample  = texture2D( oz_Masks, exTexCoord );
#endif

  vec3  base         = colourSample.xyz;
  vec3  ambient      = oz_CaelumLight.ambient;
  vec3  diffuse      = oz_CaelumLight.diffuse * max( 0.0, dot( oz_CaelumLight.dir, normal ) );
#ifdef OZ_LOW_DETAIL
  vec3  lighting     = ambient + diffuse;
#else
  float specularDot  = max( 0.0, dot( oz_CaelumLight.dir, reflectDir ) );
  vec3  emission     = vec3( masksSample.g, masksSample.g, masksSample.g );
  vec3  specular     = oz_CaelumLight.diffuse * vec3( 2.0*masksSample.r * specularDot*specularDot );
  vec3  lighting     = min( ambient + diffuse + emission, vec3( 1.25 ) ) + specular;
# ifdef OZ_ENV_MAP
  if( masksSample.b != 0.0 ) {
    vec3 environment  = textureCube( oz_EnvMap, reflectDir ).xyz;
    base              = mix( base, environment, masksSample.b );
  }
# endif
#endif

  vec4  fragColour   = vec4( base * lighting, colourSample.w );

  gl_FragData[0]     = applyFog( oz_Colour * fragColour, dist );
#ifdef OZ_POSTPROCESS
  gl_FragData[1]     = vec4( specular + emission, 1.0 );
#endif
}
