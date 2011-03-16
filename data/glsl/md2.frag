/*
 *  md2.frag
 *
 *  Mesh shader that reads in interpolates vertex positions from the given vertex texture.
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

varying vec3 exPosition;
varying vec2 exTexCoord;
varying vec4 exColour;

void main()
{
  float dist = length( exPosition );

  if( dist >= oz_Fog.end ) {
    discard;
  }

  gl_FragColor = exColour;
  gl_FragColor *= texture2D( oz_Textures[0], exTexCoord * oz_TextureScales[0] );
  gl_FragColor *= oz_Colour;
  gl_FragColor = applyFog( gl_FragColor, dist );
}
