/*
 *  stars.vert
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

in vec3 inPosition;

out vec3 exPosition;
out vec3 exLocalPosition;

void main()
{
  vec4 position   = gl_ModelViewMatrix * vec4( inPosition, 1.0 );
  exPosition      = position.xyz;
  exLocalPosition = inPosition;
  gl_Position     = oz_Transform.complete * vec4( inPosition, 1.0 );
}
