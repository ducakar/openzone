/*
 *  terraWater.vert
 *
 *  Terrain water (sea) shader.
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

uniform float oz_WaveBias;

attribute vec3 inPosition;
attribute vec2 inTexCoord;
attribute vec3 inNormal;

varying vec2 exTexCoord;

void main()
{
  float z = 0.15 * sin( oz_WaveBias + inPosition.x + inPosition.y );
  vec4 localPos = vec4( inPosition.x, inPosition.y, z, 1.0 );
  vec3 position = ( oz_Transform.model * localPos ).xyz;

  gl_Position = oz_Transform.complete * localPos;
  exTexCoord  = inTexCoord;
}
