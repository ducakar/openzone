/*
 *  stars.vert
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

attribute vec3 inPosition;

varying vec3  exPosition;
varying float exAzimuth;

void main()
{
  exPosition  = inPosition;
  exAzimuth   = ( oz_Transform.model * vec4( inPosition, 1.0 ) ).z;
  gl_Position = oz_Transform.complete * vec4( inPosition, 1.0 );
}
