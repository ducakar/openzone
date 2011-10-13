/*
 *  bigTerraLand.vert
 *
 *  Terrain (land) shader.
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

attribute vec3 inPosition;
attribute vec2 inTexCoord;
attribute vec3 inNormal;

varying vec2  exTexCoord;
varying vec3  exNormal;
varying float exDistance;

void main()
{
  vec3 position = ( oz_Transform.model * vec4( inPosition, 1.0 ) ).xyz;
  vec3 normal   = ( oz_Transform.model * vec4( inNormal, 0.0 ) ).xyz;
  vec3 toCamera = oz_CameraPosition - position;

  exTexCoord    = inTexCoord;
  exNormal      = inNormal;
  exDistance    = length( toCamera );
  gl_Position   = oz_Transform.complete * vec4( inPosition, 1.0 );
}
