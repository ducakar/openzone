/*
 *  default.frag
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#version 130

struct Transform
{
  mat4 proj;
  mat4 camera;
  mat4 model;

  mat4 cameraProj;
  mat4 complete;
};

struct SkyLight
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

/*
 * Transformation
 */
uniform Transform oz_Transform = {
  mat4( vec4( 1, 0, 0, 0 ), vec4( 0, 1, 0, 0 ), vec4( 0, 0, 1, 0 ), vec4( 0, 0, 0, 1 ) ),
  mat4( vec4( 1, 0, 0, 0 ), vec4( 0, 1, 0, 0 ), vec4( 0, 0, 1, 0 ), vec4( 0, 0, 0, 1 ) ),
  mat4( vec4( 1, 0, 0, 0 ), vec4( 0, 1, 0, 0 ), vec4( 0, 0, 1, 0 ), vec4( 0, 0, 0, 1 ) ),

  mat4( vec4( 1, 0, 0, 0 ), vec4( 0, 1, 0, 0 ), vec4( 0, 0, 1, 0 ), vec4( 0, 0, 0, 1 ) ),
  mat4( vec4( 1, 0, 0, 0 ), vec4( 0, 1, 0, 0 ), vec4( 0, 0, 1, 0 ), vec4( 0, 0, 0, 1 ) )
};

/*
 * Colour
 */
uniform vec4      oz_Colour           = vec4( 1.0, 1.0, 1.0, 1.0 );

/*
 * Texturing
 */
uniform bool      oz_IsTextureEnabled = false;
uniform sampler2D oz_Textures[2];
uniform float     oz_TextureScales[2] = { 1.0, 1.0 };

/*
 * Lighting
 */
uniform vec3      oz_SpecularMaterial;

uniform SkyLight  oz_SkyLight;
uniform Light     oz_PointLights[8];

/*
 * Distance after which far shaders are used
 */
uniform float     oz_NearDistance = 100.0;

/*
 * Fog
 */
uniform float     oz_FogDistance;
uniform vec4      oz_FogColour;

/*
 * MD2 animation
 */
uniform vec3      oz_MD2Anim;   // vec3( firstFrame, secondFrame, interpolation )

vec4 skyLightColour( in vec3 normal )
{
  float diffuseFactor = max( dot( oz_SkyLight.dir, normal ), 0.0 );
  vec4 colour = diffuseFactor * oz_SkyLight.diffuse + oz_SkyLight.ambient;
  return min( colour, vec4( 1.0, 1.0, 1.0, 1.0 ) );
}
