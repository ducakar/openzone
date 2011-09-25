/*
 *  celestial.frag
 *
 *  Shader for celestial bodies (except stars).
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

varying vec2  exTexCoord;
varying float exAzimuth;

void main()
{
  if( exAzimuth < 0.0 ) {
    discard;
  }

  gl_FragData[0] = texture2D( oz_Textures[0], exTexCoord );
  gl_FragData[0] *= oz_Colour;
}
