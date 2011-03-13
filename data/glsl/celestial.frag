/*
 *  celestial.frag
 *
 *  Shader for celestial bodies (except stars).
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

in vec2  exTexCoord;
in float exAzimuth;

out vec4 outColour;

void main()
{
  if( exAzimuth < 0.0 ) {
    discard;
  }

  outColour = texture2D( oz_Textures[0], exTexCoord );
  outColour *= oz_Colour;
}
