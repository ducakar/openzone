/*
 *  mesh.frag
 *
 *  Generic shader for meshes.
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

  outColour = oz_Colour;
  outColour *= skyLightColour( exNormal );
  outColour *= specularColour( normalize( exNormal ), toCamera / dist );
  outColour *= texture2D( oz_Textures[0], exTexCoord );
  outColour = applyFog( outColour, dist );
}
