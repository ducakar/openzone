/*
 *  default.frag
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#version 130

/*
 * Transformation
 */
struct Transform
{
  mat4 proj;
  mat4 camera;
  mat4 model;

  mat4 cameraProj;
  mat4 complete;
};

uniform Transform oz_Transform;

/*
 * Colour
 */
uniform vec4      oz_Colour             = vec4( 1.0, 1.0, 1.0, 1.0 );

/*
 * Texturing
 */
uniform bool      oz_IsTextureEnabled   = false;
uniform sampler2D oz_Textures[2];
uniform float     oz_TextureScales[2]   = { 1.0, 1.0 };

/*
 * Lighting
 */
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

uniform vec3      oz_SpecularMaterial;

uniform SkyLight  oz_SkyLight;
uniform Light     oz_PointLights[8];

/*
 * Fog
 */
struct Fog {
  float start;
  float end;
  vec4  colour;
};

uniform Fog       oz_Fog;

/*
 * Water
 */
uniform float     oz_WaveBias;

/*
 * MD2 animation
 */
uniform vec3      oz_MD2Anim;   // vec3( firstFrame, secondFrame, interpolation )

/*
 * FUNCTIONS
 */

vec4 greyscale( in vec4 colour )
{
  float avg = ( colour.x + colour.y + colour.z ) / 3.0;
  return vec4( avg, avg, avg, colour.w );
}

vec4 skyLightColour( in vec3 normal )
{
  float diffuseFactor = max( dot( oz_SkyLight.dir, normal ), 0.0 );
  vec4 colour = diffuseFactor * oz_SkyLight.diffuse + oz_SkyLight.ambient;
  return min( colour, vec4( 1.0, 1.0, 1.0, 1.0 ) );
}

vec4 applyFog( in vec4 colour, in float dist )
{
  float ratio = ( dist - oz_Fog.start ) / ( oz_Fog.end - oz_Fog.start );
  return mix( colour, oz_Fog.colour, clamp( ratio, 0.0, 1.0 ) );
}
