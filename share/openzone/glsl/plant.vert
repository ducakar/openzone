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

out vec3 exPosition;
out vec2 exTexCoord;
out vec3 exNormal;

void main()
{
  exPosition     = ( oz_Transform.model * vec4( inPosition, 1.0 ) ).xyz;
  exNormal       = ( oz_Transform.model * vec4( inNormal, 0.0 ) ).xyz;

  float windFact = max( inPosition.z, 0.0 );
  vec2  windBias = oz_Wind.xy * windFact*windFact * oz_Wind.z *
      sin( 0.08 * ( exPosition.x + exPosition.y ) + oz_Wind.w );

  exTexCoord     = inTexCoord;
  gl_Position    = oz_Transform.complete * vec4( inPosition.xy + windBias.xy, inPosition.z, 1.0 );
}
