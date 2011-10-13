/*
 *  combine.vert
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

attribute vec3 inPosition;
attribute vec2 inTexCoord;

varying vec2 exTexCoord;

void main()
{
  gl_Position = oz_Transform.complete * vec4( inPosition, 1.0 );
  exTexCoord  = inTexCoord;
}
