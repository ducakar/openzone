/*
 *  terraWater.frag
 *
 *  Terrain water (sea) shader.
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

in vec3 exPosition;
in vec2 exTexCoord;

out vec4 outColour;

void main()
{
  vec3 toCamera = oz_CameraPosition - exPosition;
  float dist = length( toCamera );

  outColour = vec4( 1.0, 1.0, 1.0, 0.75 );
  outColour *= skyLightColour( vec3( 0.0, 0.0, 1.0 ) );
  outColour *= texture( oz_Textures[0], exTexCoord * oz_TextureScales[0] );
  outColour = applyFog( outColour, dist );
}
