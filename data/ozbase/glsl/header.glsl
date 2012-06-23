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
 * header.glsl
 *
 * Common include file, prepended before all shader sources.
 */

precision lowp int;
precision lowp float;

const float TAU = 6.283185307179586;

/*
 * Transformation
 */
uniform mat4            oz_ProjModelTransform;
uniform mat4            oz_ModelTransform;
uniform mat4            oz_BoneTransforms[16];

uniform vec3            oz_CameraPosition;

/*
 * Colour
 */
uniform vec4            oz_Colour;

/*
 * Texturing { albedo, masks, normals, frames positions, frame normals }
 */
uniform sampler2D       oz_Textures[6];

/*
 * Lighting
 */
struct CaelumLight
{
  vec3 dir;
  vec4 diffuse;
  vec4 ambient;
};

struct Light
{
  vec3 pos;
  vec4 diffuse;
};

uniform CaelumLight     oz_CaelumLight;
uniform Light           oz_PointLights[8];

uniform bool            oz_NightVision;

/*
 * Fog
 */
struct Fog
{
  float dist;
  vec4  colour;
};

uniform Fog             oz_Fog;

/*
 * FUNCTIONS
 */

vec4 greyscale( vec4 colour )
{
  float avg = dot( colour, vec4( 0.333, 0.333, 0.333, 0.0 ) );
  return vec4( avg, avg, avg, colour.a );
}

vec4 skyLightColour( vec3 normal )
{
  float diffuseFactor = max( dot( -oz_CaelumLight.dir, normal ), 0.0 );
  vec4 colour = diffuseFactor * oz_CaelumLight.diffuse + oz_CaelumLight.ambient;
  return min( colour, vec4( 1.0, 1.0, 1.0, 1.0 ) );
}

vec4 specularColour( float specular, vec3 normal, vec3 toCamera )
{
  vec3 reflectedLight = reflect( oz_CaelumLight.dir, normal );
  float factor = 2.0 * specular * max( dot( reflectedLight, toCamera ), 0.0 );
  return factor * oz_CaelumLight.diffuse;
}

vec4 applyFog( vec4 colour, float dist )
{
  float ratio = min( dist / oz_Fog.dist, 1.0 );
  return mix( colour, oz_Fog.colour, ratio*ratio );
}














































































// this file should have a 'round' number of lines to easier determine error line from compiler
// output as it is biased by the number of lines in header.glsl file
