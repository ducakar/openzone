/*
 *  ui.vert
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

void main()
{
  gl_FrontColor  = gl_Color;
  gl_TexCoord[0] = gl_MultiTexCoord0;
  gl_Position    = ftransform();
}
