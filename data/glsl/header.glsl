/*
 *  default.frag
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

/*
 * Texturing
 */
uniform bool      oz_IsTextureEnabled;
uniform sampler2D oz_Textures[2];
uniform float     oz_TextureScales[2];

/*
 * Lighting
 */
uniform vec4      oz_DiffuseMaterial;
uniform vec4      oz_SpecularMaterial;

// vec3 dir, vec3 diffuse, vec3 ambient
uniform vec3      oz_SkyLight[3];

// vec3 pos, vec3 diffuse
uniform vec3      oz_PointLights[16];

/*
 * Fog
 */
uniform float     oz_FogDistance;
uniform vec4      oz_FogColour;

/*
 * Highlight (selected object)
 */
const   vec3      oz_HighlightBase   = vec3( 0.00, 0.30, 0.40 );
const   vec3      oz_HighlightFactor = vec3( 0.40, 1.20, 1.60 );

uniform bool      oz_IsHighlightEnabled;
