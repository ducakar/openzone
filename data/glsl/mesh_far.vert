/*
 *  mesh_far.vert
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

attribute vec3 oz_Position;
attribute vec2 oz_TexCoord;
attribute vec3 oz_Normal;

varying vec2 texCoord;
varying float fogRatio;
varying vec3 skyLightColour;

void main()
{
  vec4  position = gl_ModelViewMatrix * vec4( oz_Position, 1.0 );
  vec3  normal   = gl_NormalMatrix * oz_Normal;
  float dist     = length( position );

  gl_Position    = gl_ProjectionMatrix * position;
  texCoord       = oz_TexCoord;
  fogRatio       = min( dist / oz_FogDistance, 1.0 );

  float diffuseFactor = clamp( dot( oz_SkyLight[0], normal ), 0.0, 1.0 );
  skyLightColour = diffuseFactor * oz_SkyLight[1] + oz_SkyLight[2];
  skyLightColour = clamp( skyLightColour, vec3( 0.0, 0.0, 0.0 ), vec3( 1.0, 1.0, 1.0 ) );
}
