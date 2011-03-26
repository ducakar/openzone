/*
 *  bigMesh.frag
 *
 *  Same as mesh shader, but per-pixel fog as BSPs can have quite large faces.
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

in vec2  exTexCoord;
in float exDistance;
in vec4  exColour;

out vec4 outColour;

void main()
{
  outColour = exColour;
  outColour *= texture( oz_Textures[0], exTexCoord );
  outColour = applyFog( outColour, exDistance );
}
