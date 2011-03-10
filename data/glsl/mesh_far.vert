/*
 *  mesh_far.vert
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

in vec3 inPosition;
in vec2 inTexCoord;
in vec3 inNormal;

out vec2  exTexCoord;
out float exFogRatio;
out vec4  exColour;

void main()
{
  vec4  position = oz_Transform.camera * oz_Transform.model * vec4( inPosition, 1.0 );
  vec3  normal   = ( oz_Transform.model * vec4( inNormal, 0.0 ) ).xyz;
  float dist     = max( length( position ) - oz_NearDistance, 0.0 );

  gl_Position    = oz_Transform.complete * vec4( inPosition, 1.0 );
  exTexCoord     = inTexCoord;
  exFogRatio     = min( dist / ( oz_FogDistance - oz_NearDistance ), 1.0 );
  exColour       = skyLightColour( normal );
}
