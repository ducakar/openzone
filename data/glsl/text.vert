/*
 *  text.vert
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

in vec4 inPosition;

void main()
{
  gl_Position = oz_Transform.proj * inPosition;
}
