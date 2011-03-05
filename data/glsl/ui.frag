/*
 *  ui.frag
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

varying vec2 outTexCoord;

void main()
{
  if( oz_IsTextureEnabled != 0 ) {
    gl_FragColor = texture2D( oz_Textures[0], outTexCoord );
  }
  else {
    gl_FragColor = gl_Color;
  }
}
