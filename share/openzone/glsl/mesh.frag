/*
 *  mesh.frag
 *
 *  Generic shader for meshes.
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

varying vec3 exPosition;
varying vec2 exTexCoord;
varying vec3 exNormal;

void main()
{
  vec3 toCamera = oz_CameraPosition - exPosition;
  vec3 normal   = normalize( exNormal );
  float dist    = length( toCamera );

  gl_FragData[0] = oz_Colour;
  gl_FragData[0] *= skyLightColour( normal );
  gl_FragData[0] *= specularColour( normal, toCamera / dist );
  gl_FragData[0] *= texture2D( oz_Textures[0], exTexCoord );
  gl_FragData[0] = applyFog( gl_FragData[0], dist );
}
