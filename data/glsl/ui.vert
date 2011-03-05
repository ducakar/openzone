/*
 *  ui.vert
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

attribute vec3 inPosition;
attribute vec2 inTexCoord;

varying vec2 outTexCoord;

void main()
{
  gl_FrontColor = gl_Color;
  outTexCoord   = inTexCoord;
  gl_Position   = gl_ModelViewProjectionMatrix * vec4( inPosition, 1.0 );
}
