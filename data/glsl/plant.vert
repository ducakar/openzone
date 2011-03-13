/*
 *  plant.vert
 *
 *  Mesh shader that deforms mesh according to the given wind.
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
  vec3 position  = ( oz_Transform.camera * oz_Transform.model * vec4( inPosition, 1.0 ) ).xyz;
  vec3 normal    = ( oz_Transform.model * vec4( inNormal, 0.0 ) ).xyz;

  exTexCoord     = inTexCoord;
  exColour       = skyLightColour( normal );
  exDistance     = length( position );

  vec4  worldPos = oz_Transform.model * vec4( inPosition, 1.0 );
  float windFact = max( inPosition.z, 0.0 );
  vec2  windBias = oz_Wind.xy * windFact*windFact * oz_Wind.z *
      sin( 0.02 * ( worldPos.x + worldPos.y ) + oz_Wind.w );
  gl_Position    = oz_Transform.complete * vec4( inPosition.xy + windBias.xy, inPosition.z, 1.0 );
}
