/*
 *  itemview.vert
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

attribute vec3 oz_Position;
attribute vec2 oz_TexCoord;

varying   vec2 oz_FragTexCoord;

void main()
{
  gl_Position     = gl_ModelViewProjectionMatrix * vec4( oz_Position, 1.0 );
  oz_FragTexCoord = oz_TexCoord;
}
