/*
 *  stars.frag
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

varying vec3  exPosition;
varying float exAzimuth;

void main()
{
  if( exAzimuth < 0.0 ) {
    discard;
  }

  float dist    = ( length( exPosition ) - 50.0 ) / 50.0;
  vec4  diff    = oz_Colour - oz_Fog.colour;
  vec4  disturb = vec4( sin( exPosition * 100.0 ) * 0.1, 0.0 ) * dot( diff, diff );

  gl_FragData[0] = mix( oz_Colour + disturb, oz_Fog.colour, dist );
}
