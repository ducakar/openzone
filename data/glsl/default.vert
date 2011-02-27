/*
 *  default.vert
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

varying vec3 oz_TransformedPointLights[16];

varying vec3 oz_Position;
varying vec3 oz_Normal;

void main()
{
  gl_FrontColor  = gl_Color;
  gl_TexCoord[0] = gl_MultiTexCoord0;
  gl_TexCoord[1] = gl_MultiTexCoord1;
  gl_Position    = ftransform();

  oz_Position    = ( gl_ModelViewMatrix * gl_Position ).xyz;
  oz_Normal      = gl_NormalMatrix * gl_Normal;

  oz_TransformedPointLights[0] = ( gl_ModelViewMatrix * vec4( oz_PointLights[0], 1.0 ) ).xyz;
}
