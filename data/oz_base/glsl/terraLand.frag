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

#version 100

precision mediump float;

const float DETAIL_SCALE  = 1024.0;
const float NORMALS_SCALE = 512.0;

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

  normal             = plane * pixelNormal( oz_Normals, exTexCoord * NORMALS_SCALE );
#endif
#ifdef OZ_LOW_DETAIL
  float dist         = 1.0 / gl_FragCoord.w;
#else
  float dist         = length( exLook );
#endif

  vec4  detailSample = texture2D( oz_Texture, exTexCoord * DETAIL_SCALE );
  vec4  mapSample    = texture2D( oz_Masks, exTexCoord );

  vec3  base         = detailSample.xyz * mapSample.xyz;
  vec3  ambient      = oz_CaelumLight.ambient;
  vec3  diffuse      = oz_CaelumLight.diffuse * max( 0.0, dot( oz_CaelumLight.dir, normal ) );
  vec3  lighting     = min( ambient + diffuse, vec3( 1.25 ) );
  vec4  fragColour   = vec4( base * lighting, 1.0 );

  gl_FragData[0]     = applyFog( oz_Colour * fragColour, dist );
#ifdef OZ_POSTPROCESS
  gl_FragData[1]     = vec4( 0.0, 0.0, 0.0, 1.0 );
#endif
}
