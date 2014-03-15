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
 * mesh.frag
 *
 * Generic shader for meshes.
 */

#version 100

precision mediump float;

struct CaelumLight
{
  vec3 dir;
  vec3 ambient;
  vec3 colour;
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

varying vec3 exLook;
varying vec2 exTexCoord;
varying vec3 exNormal;
#ifdef OZ_BUMP_MAP
varying vec3 exTangent;
varying vec3 exBinormal;
#endif

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
#ifdef OZ_BUMP_MAP
  mat3  plane        = mat3( exTangent, exBinormal, exNormal );
  vec3  normal       = normalize( plane * pixelNormal( oz_Normals, exTexCoord ) );
#else
  vec3  normal       = normalize( exNormal );
#endif
  float dist         = length( exLook );
  vec3  reflectDir   = reflect( exLook / dist, normal );

  float diffuseDot   = max( 0.0, dot( oz_CaelumLight.dir, normal ) );
  float specularDot  = max( 0.0, dot( oz_CaelumLight.dir, reflectDir ) );
  vec4  colour       = texture2D( oz_Texture, exTexCoord );
  vec4  masks        = texture2D( oz_Masks, exTexCoord );
  vec3  ambient      = oz_CaelumLight.ambient;
  vec3  diffuse      = oz_CaelumLight.colour * diffuseDot;
  vec3  emission     = masks.ggg;
  vec3  specular     = oz_CaelumLight.colour * ( masks.r * pow( specularDot, 32.0 ) );
#ifdef OZ_ENV_MAP
  vec3  environment  = textureCube( oz_EnvMap, reflectDir ).rgb;

  colour.rgb         = mix( colour.rgb, environment, masks.b );
#endif
  colour.rgb         = colour.rgb * ( ambient + diffuse + emission ) + specular;

  gl_FragData[0]     = oz_Colour * applyFog( colour, dist );
#ifdef OZ_POSTPROCESS
  gl_FragData[1]     = vec4( specular + emission, 1.0 );
#endif
}
