/*
 *  bigMesh.vert
 *
 *  Same as mesh shader, but per-pixel fog as BSPs can have quite large faces.
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

attribute vec3 inPosition;
attribute vec2 inTexCoord;
attribute vec3 inNormal;

varying vec2  exTexCoord;
varying float exDistance;
varying vec4  exColour;

void main()
{
  vec3 position = ( oz_Transform.model * vec4( inPosition, 1.0 ) ).xyz;
  vec3 normal   = ( oz_Transform.model * vec4( inNormal, 0.0 ) ).xyz;
  vec3 toCamera = oz_CameraPosition - position;

  exTexCoord    = inTexCoord;
  exDistance    = length( toCamera );
  exColour      = oz_Colour;
  exColour      *= skyLightColour( normal );
  exColour      *= specularColour( normal, toCamera / exDistance );
  gl_Position   = oz_Transform.complete * vec4( inPosition, 1.0 );
}
