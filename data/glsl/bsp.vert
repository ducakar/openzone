/*
 *  bsp.vert
 *
 *  Same as mesh shader, but per-pixel fog as BSPs can have quite large faces.
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

attribute vec3 inPosition;
attribute vec2 inTexCoord;
attribute vec3 inNormal;

varying vec3 exPosition;
varying vec2 exTexCoord;
varying vec4 exColour;

void main()
{
  vec3 normal    = ( oz_Transform.model * vec4( inNormal, 0.0 ) ).xyz;

  exPosition     = ( oz_Transform.camera * oz_Transform.model * vec4( inPosition, 1.0 ) ).xyz;
  exTexCoord     = inTexCoord;
  exColour       = skyLightColour( normal );

  gl_Position    = oz_Transform.complete * vec4( inPosition, 1.0 );
}
