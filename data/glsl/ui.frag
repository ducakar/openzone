/*
 *  ui.frag
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

void main()
{
  if( oz_TextureEnabled != 0 ) {
    gl_FragColor = texture2D( oz_Textures[0], gl_TexCoord[0].st );
  }
  else {
    gl_FragColor = gl_Color;
  }
}
