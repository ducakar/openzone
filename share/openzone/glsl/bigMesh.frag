/*
 *  bigMesh.frag
 *
 *  Same as mesh shader, but per-pixel fog as BSPs can have quite large faces.
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

varying vec2  exTexCoord;
varying float exDistance;
varying vec4  exColour;

void main()
{
  gl_FragData[0] = exColour;
  gl_FragData[0] *= texture2D( oz_Textures[0], exTexCoord );
  gl_FragData[0] = applyFog( gl_FragData[0], exDistance );
}
