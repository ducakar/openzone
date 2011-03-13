/*
 *  md2.frag
 *
 *  Mesh shader that reads in interpolates vertex positions from the given vertex texture.
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

in vec3 exPosition;
in vec2 exTexCoord;
in vec4 exColour;

out vec4 outColour;

void main()
{
  float dist = length( exPosition );

  if( dist >= oz_Fog.end ) {
    discard;
  }

  outColour = exColour;
  outColour *= texture2D( oz_Textures[0], exTexCoord * oz_TextureScales[0] );
  outColour *= oz_Colour;
  outColour = applyFog( outColour, dist );
}
