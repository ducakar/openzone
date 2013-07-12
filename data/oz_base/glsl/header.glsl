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
 * header.glsl
 *
 * Common include file, prepended before all shader sources.
 */

#ifdef GL_ES
precision lowp int;
precision lowp float;
#endif

const float TAU = 6.283185307179586;

/*
 * Transformation
 */
uniform mat4            oz_ProjModelTransform;
uniform mat4            oz_ModelTransform;
uniform mat4            oz_BoneTransforms[16];

/*
 * Colour
 */
uniform mat4            oz_ColourTransform;

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
  vec3 diffuse;
  vec3 ambient;
};

struct Light
{
  vec3 pos;
  vec3 diffuse;
};

uniform CaelumLight     oz_CaelumLight;
uniform Light           oz_PointLights[8];
uniform vec3            oz_CameraPosition;

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

vec4 applyFog( vec4 colour, float dist )
{
  float ratio = min( dist / oz_Fog.dist, 1.0 );
  return mix( colour, oz_Fog.colour, ratio*ratio );
}
