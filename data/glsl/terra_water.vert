/*
 *  terra_near.vert
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

in vec3 inPosition;
in vec2 inTexCoord;
in vec3 inNormal;

out vec4 exPosition;
out vec2 exTexCoord;
out vec4 exColour;

void main()
{
  exPosition     = oz_Transform.camera * oz_Transform.model * vec4( inPosition, 1.0 );

  vec3  normal   = ( oz_Transform.model * vec4( inNormal, 0.0 ) ).xyz;
  float dist     = max( length( exPosition ) - oz_NearDistance, 0.0 );

  gl_Position    = oz_Transform.complete * vec4( inPosition, 1.0 );
  exTexCoord     = inTexCoord;
  exColour       = skyLightColour( normal );
}
