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

out vec2  exTexCoord;
out vec4  exColour;
out float exDistance;

void main()
{
  float z = 0.15 * sin( oz_WaveBias + inPosition.x + inPosition.y );

  vec4 localPos  = vec4( inPosition.x, inPosition.y, z, 1.0 );
  vec4 position  = oz_Transform.camera * oz_Transform.model * localPos;
  vec3  normal   = ( oz_Transform.model * vec4( 0.0, 0.0, 1.0, 0.0 ) ).xyz;

  gl_Position    = oz_Transform.complete * localPos;
  exTexCoord     = inTexCoord;
  exColour       = skyLightColour( normal );
  exDistance     = length( position );
}
