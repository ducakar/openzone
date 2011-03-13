/*
 *  terra_land.frag
 *
 *  Terrain (land) shader.
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

in vec2  exTexCoord;
in vec4  exColour;
in float exDistance;

out vec4 outColour;

void main()
{
  if( exDistance >= oz_Fog.end ) {
    discard;
  }

  outColour = exColour;
  outColour *= texture2D( oz_Textures[0], exTexCoord * oz_TextureScales[0] );
  outColour *= texture2D( oz_Textures[1], exTexCoord * oz_TextureScales[1] );
  outColour = applyFog( outColour, exDistance );
}
