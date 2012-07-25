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
in vec3 exNormal;

out vec4 outColour;

void main()
{
  vec3 toCamera = oz_CameraPosition - exPosition;
  vec3 normal = normalize( exNormal );
  float dist = length( toCamera );

  outColour = oz_Colour;
  outColour *= skyLightColour( normal );
  outColour *= specularColour( normal, toCamera / dist );
  outColour *= texture( oz_Textures[0], exTexCoord );
  outColour = applyFog( outColour, dist );
}
