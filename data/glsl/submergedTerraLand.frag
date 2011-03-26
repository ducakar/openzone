/*
 *  terraLand.frag
 *
 *  Terrain (land) shader.
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

in vec3 exPosition;
in vec2 exTexCoord;
in vec3 exNormal;

out vec4 outColour;

void main()
{
  vec3 toCamera = oz_CameraPosition - exPosition;
  float dist = length( toCamera );

  outColour = skyLightColour( exNormal );
  outColour *= texture( oz_Textures[0], exTexCoord * oz_TextureScales[0] );
  outColour *= texture( oz_Textures[1], exTexCoord * oz_TextureScales[1] );
  outColour = applyFog( outColour, dist );
}
