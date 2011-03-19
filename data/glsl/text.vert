/*
 *  text.vert
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

in vec3 inPosition;

void main()
{
  gl_Position = oz_Transform.proj * vec4( inPosition, 1.0 );
}
