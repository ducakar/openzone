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

#ifndef GL_ES
# define lowp
# define mediump
# define highp
#endif

const lowp float TAU = 6.283185307179586;

/*
 * Transformation
 */
struct Transform
{
  lowp mat4 model;
  lowp mat4 complete;
};

uniform Transform       oz_Transform;

uniform lowp vec3       oz_CameraPosition;

/*
 * Colour
 */
uniform lowp vec4       oz_Colour;

/*
 * Texturing { albedo, masks, normals, frames positions, frame normals }
 */
uniform sampler2D       oz_Textures[5];

/*
 * Lighting
 */
struct CaelumLight
{
  lowp vec3 dir;
  lowp vec4 diffuse;
  lowp vec4 ambient;
};

struct Light
{
  lowp vec3 pos;
  lowp vec4 diffuse;
};

uniform CaelumLight     oz_CaelumLight;
uniform Light           oz_PointLights[8];

uniform bool            oz_NightVision;

/*
 * Fog
 */
struct Fog
{
  lowp float dist;
  lowp vec4  colour;
};

uniform Fog             oz_Fog;

/*
 * FUNCTIONS
 */

lowp vec4 greyscale( lowp vec4 colour )
{
  lowp float avg = dot( colour, vec4( 0.333, 0.333, 0.333, 0.0 ) );
  return vec4( avg, avg, avg, colour.a );
}

lowp vec4 skyLightColour( lowp vec3 normal )
{
  lowp float diffuseFactor = max( dot( -oz_CaelumLight.dir, normal ), 0.0 );
  lowp vec4 colour = diffuseFactor * oz_CaelumLight.diffuse + oz_CaelumLight.ambient;
  return min( colour, vec4( 1.0, 1.0, 1.0, 1.0 ) );
}

lowp vec4 specularColour( lowp float specular, lowp vec3 normal, lowp vec3 toCamera )
{
  lowp vec3 reflectedLight = reflect( oz_CaelumLight.dir, normal );
  lowp float factor = 2.0 * specular * max( dot( reflectedLight, toCamera ), 0.0 );
  return factor * oz_CaelumLight.diffuse;
}

lowp vec4 applyFog( lowp vec4 colour, lowp float dist )
{
  lowp float ratio = min( dist / oz_Fog.dist, 1.0 );
  return mix( colour, oz_Fog.colour, ratio*ratio );
}







































































// this file should have a 'round' number of lines to easier determine error line from compiler
// output as it is biased by the number of lines in header.glsl file
