/*
 *  simple.frag
 *
 *  Shader for bounding boxes, aim dot etc.
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

varying vec2 exTexCoord;

void main()
{
  if( oz_IsTextureEnabled ) {
    gl_FragData[0] = texture2D( oz_Textures[0], exTexCoord );
  }
  else {
    gl_FragData[0] = oz_Colour;
  }
}
