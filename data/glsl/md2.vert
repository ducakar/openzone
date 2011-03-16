/*
 *  md2.vert
 *
 *  Mesh shader that reads in interpolates vertex positions from the given vertex texture.
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

  vec4 firstPos  = texture2DLod( oz_Textures[1], vec2( oz_MD2Anim[0], inPosition.x ), 0.0 );
  vec4 secondPos = texture2DLod( oz_Textures[1], vec2( oz_MD2Anim[1], inPosition.x ), 0.0 );
  gl_Position    = oz_Transform.complete * mix( firstPos, secondPos, oz_MD2Anim[2] );
}
