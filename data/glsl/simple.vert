/*
 *  simple.vert
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

in vec3 inPosition;
in vec2 inTexCoord;

out vec2 exTexCoord;

void main()
{
  exTexCoord  = inTexCoord;
  gl_Position = oz_Transform.complete * vec4( inPosition, 1.0 );
}
