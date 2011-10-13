/*
 *  plant.vert
 *
 *  Mesh shader that deforms mesh according to the given wind.
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

// vec4( vec2( dirXY ), amplitude, phi )
uniform vec4 oz_Wind;

attribute vec3 inPosition;
attribute vec2 inTexCoord;
attribute vec3 inNormal;

varying vec2 exTexCoord;
varying vec4 exNormal;

void main()
{
  float windFact = max( inPosition.z, 0.0 );
  vec2  windBias = oz_Wind.xy * windFact*windFact * oz_Wind.z *
      sin( 0.08 * ( inPosition.x + inPosition.y ) + oz_Wind.w );

  gl_Position = oz_Transform.complete * vec4( inPosition.xy + windBias.xy, inPosition.z, 1.0 );
  exTexCoord  = inTexCoord;
  exNormal    = oz_Transform.complete * vec4( inNormal, 0.0 );
}
