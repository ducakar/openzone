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

precision lowp int;
precision lowp float;

const float TAU = 6.283185307179586;

/*
 * Transformation
 */
uniform mat4            oz_ProjCamera;
uniform mat4            oz_Model;
uniform mat3            oz_ModelRot;
uniform vec3            oz_CameraPos;
uniform mat4            oz_Bones[16];

/*
 * Colour
 */
uniform mat4            oz_Colour;

/*
 * Textures
 */
uniform sampler2D       oz_Texture;
uniform sampler2D       oz_Masks;
uniform sampler2D       oz_Normals;
uniform samplerCube     oz_EnvMap;
uniform sampler2D       oz_VertexAnim;

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

/*
 * Fog
 */
struct Fog
{
  vec4  colour;
  float dist2;
};

uniform Fog             oz_Fog;

/*
 * FUNCTIONS
 */

vec3 pixelNormal( sampler2D texture, vec2 texCoord )
{
  vec3 texel = texture2D( texture, texCoord ).xyz;
  return vec3( 2.0, 2.0, 1.0 ) * texel - vec3( 1.0, 1.0, 0.0 );
}

vec4 applyFog( vec4 colour, float dist )
{
  float ratio = min( dist*dist / oz_Fog.dist2, 1.0 );
  return mix( colour, oz_Fog.colour, ratio*ratio );
}
