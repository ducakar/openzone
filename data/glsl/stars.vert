/*
 *  stars.vert
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

attribute vec3 oz_Position;

varying vec4 colour;

void main()
{
  vec4 position = gl_ModelViewMatrix * vec4( oz_Position, 1.0 );
  float dist = ( length( position ) - 50.0 ) / 50.0;

  vec4 disturb = vec4( sin( oz_Position * 100.0 ) * 0.1, 1.0 );

  colour = mix( oz_DiffuseMaterial + disturb, oz_FogColour, dist );
  gl_Position = gl_ProjectionMatrix * position;
}
