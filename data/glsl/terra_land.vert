/*
 *  terra_land.vert
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

in vec3 inPosition;
in vec2 inTexCoord;
in vec3 inNormal;

out vec2  exTexCoord;
out float exDistance;
out vec4  exColour;

void main()
{
  vec4  position = oz_Transform.camera * oz_Transform.model * vec4( inPosition, 1.0 );
  vec3  normal   = ( oz_Transform.model * vec4( inNormal, 0.0 ) ).xyz;

  gl_Position    = oz_Transform.complete * vec4( inPosition, 1.0 );
  exTexCoord     = inTexCoord;
  exColour       = skyLightColour( normal );
  exDistance     = length( position );
}
