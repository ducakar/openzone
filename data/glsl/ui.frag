/*
 *  ui.frag
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

in vec2 exTexCoord;

out vec4 outColour;

void main()
{
  if( oz_IsTextureEnabled ) {
    outColour = texture2D( oz_Textures[0], exTexCoord );
  }
  else {
    outColour = oz_Colour;
  }
}
