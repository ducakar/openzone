/*
 *  ui.frag
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

in vec2 exTexCoord;

out vec4 outColour;

void main()
{
  outColour = oz_Colour;

  if( oz_IsTextureEnabled ) {
    outColour *= texture( oz_Textures[0], exTexCoord );
  }
}
