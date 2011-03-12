/*
 *  mesh_near.frag
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

in vec4 exPosition;
in vec2 exTexCoord;
in vec4 exColour;

out vec4 outColour;

void main()
{
  float fogRatio = min( length( exPosition ) / oz_FogDistance, 1.0 );

  outColour = exColour;
  outColour *= texture2D( oz_Textures[0], exTexCoord * oz_TextureScales[0] );
  outColour *= oz_Colour;
  outColour = mix( outColour, oz_FogColour, fogRatio );
}
