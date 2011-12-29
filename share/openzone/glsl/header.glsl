/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2011 Davorin Učakar
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

const float TAU = 6.283185307179586;

/*
 * Transformation
 */
struct Transform
{
  mat4 model;
  mat4 complete;
};

uniform Transform       oz_Transform;

uniform vec3            oz_CameraPosition;

/*
 * Colour
 */
uniform vec4            oz_Colour = vec4( 1.0, 1.0, 1.0, 1.0 );

/*
 * Texturing
 */
uniform sampler2D       oz_Textures[4];

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

uniform float           oz_Specular = 0.0;

/*
 * Fog
 */
struct Fog
{
  float start;
  float end;
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
  float factor = oz_Specular * specular * max( dot( reflectedLight, toCamera ), 0.0 );
  return vec4( factor * oz_CaelumLight.diffuse.rgb, 0.0 );
}

vec4 applyFog( vec4 colour, float dist )
{
  float ratio = ( dist - oz_Fog.start ) / ( oz_Fog.end - oz_Fog.start );
  return mix( colour, oz_Fog.colour, clamp( ratio, 0.0, 1.0 ) );
}

vec2 noise( vec2 seed )
{
  return vec2( cos( seed.x ), sin( seed.y ) );
}






































































// this file should have a 'round' number of lines to easier determine error line from compiler
// output as it is biased by the number of lines in header.glsl file
