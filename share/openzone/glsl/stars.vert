/*
 *  stars.vert
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

in vec3 inPosition;

out vec3  exPosition;
out float exAzimuth;

void main()
{
  exPosition  = inPosition;
  exAzimuth   = ( oz_Transform.model * vec4( inPosition, 1.0 ) ).z;
  gl_Position = oz_Transform.complete * vec4( inPosition, 1.0 );
}
