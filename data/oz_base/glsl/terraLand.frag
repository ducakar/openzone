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
 * terraLand.frag
 *
 * Terrain (land) shader.
 */

precision highp float;

struct CaelumLight
{
  vec3 dir;
  vec3 colour;
  vec3 ambient;
};

struct Light
{
  vec3 dir;
  vec3 colour;
};

struct Fog
{
  vec3  colour;
  float distance2;
};

uniform mat4        oz_Colour;
uniform sampler2D   oz_Texture;
uniform sampler2D   oz_Masks;
uniform sampler2D   oz_Normals;
uniform int         oz_NumLights;
uniform CaelumLight oz_CaelumLight;
uniform Light       oz_Lights[8];
uniform Fog         oz_Fog;

varying vec3 exPosition;
varying vec2 exTexCoord;
varying vec3 exNormal;
#ifdef OZ_BUMP_MAP
varying vec3 exTangent;
varying vec3 exBinormal;
#endif

void main()
{
#ifdef OZ_BUMP_MAP
  mat3  planeTransf  = mat3(exTangent, exBinormal, exNormal);
  vec3  localNormal  = 2.0 * texture2D(oz_Normals, exTexCoord * 2.0).agb - vec3(1.0);
  vec3  normal       = normalize(planeTransf * localNormal);
#else
  vec3  normal       = normalize(exNormal);
#endif
  float distance2    = dot(exPosition, exPosition);
  float fog          = min(distance2 / oz_Fog.distance2, 1.0);

  vec4  detailSample = texture2D(oz_Texture, exTexCoord * 2.0);
  vec4  mapSample    = texture2D(oz_Masks, exTexCoord / 513.0);
  vec4  colour       = detailSample * mapSample;

  // Caelum light.
  float diffuseDot   = max(0.0, dot(oz_CaelumLight.dir, normal));
  vec3  ambient      = oz_CaelumLight.ambient;
  vec3  diffuse      = oz_CaelumLight.colour * diffuseDot;

  colour.rgb         = colour.rgb * (ambient + diffuse);
  colour.rgb         = mix(colour.rgb, oz_Fog.colour, fog*fog);

  gl_FragData[0]     = oz_Colour * colour;
#ifdef OZ_POSTPROCESS
  gl_FragData[1]     = vec4(0.0, 0.0, 0.0, 1.0);
#endif
}
