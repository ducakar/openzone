/*
 *  stars.frag
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

in vec3  exPosition;
in float exAzimuth;

out vec4 outColour;

void main()
{
  if( exAzimuth < 0.0 ) {
    discard;
  }

  float dist    = ( length( exPosition ) - 50.0 ) / 50.0;
  vec4  diff    = oz_Colour - oz_Fog.colour;
  vec4  disturb = vec4( sin( exPosition * 100.0 ) * 0.05, 0.0 ) * dot( diff, diff );
  outColour     = mix( oz_Colour + disturb, oz_Fog.colour, dist );
}
