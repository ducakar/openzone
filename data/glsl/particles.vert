/*
 *  particles.vert
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

attribute vec3 inPosition;
attribute vec2 inTexCoord;
attribute vec3 inNormal;

varying vec2 outTexCoord;
varying vec4 outColour;

void main()
{
  vec4  position = gl_ModelViewMatrix * vec4( inPosition, 1.0 );
  vec3  normal   = gl_NormalMatrix * inNormal;
  float dist     = length( position );
  float fogRatio = min( dist / oz_FogDistance, 1.0 );

  outColour      = gl_Color;

  float diffuseFactor = clamp( dot( oz_SkyLight[0], normal ), 0.0, 1.0 );
  outColour.xyz  *= diffuseFactor * oz_SkyLight[1] + oz_SkyLight[2];
  outColour.xyz  = clamp( outColour.xyz, vec3( 0.0, 0.0, 0.0 ), vec3( 1.0, 1.0, 1.0 ) );
  outColour      = mix( outColour, oz_FogColour, fogRatio );

  gl_Position    = gl_ProjectionMatrix * position;
}
