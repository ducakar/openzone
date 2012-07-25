/*
 *  plant.frag
 *
 *  Mesh shader that deforms mesh according to the given wind.
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

varying vec2 exTexCoord;
varying vec4 exNormal;

void main()
{
  gl_FragData[0] = oz_Colour * texture2D( oz_Textures[0], exTexCoord );
  gl_FragData[1] = exNormal;
}
