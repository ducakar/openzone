/*
 *  terraLand.frag
 *
 *  Terrain (land) shader.
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
  float dist = length( toCamera );

  gl_FragData[0] = skyLightColour( exNormal );
  gl_FragData[0] *= texture2D( oz_Textures[0], exTexCoord * TERRA_DETAIL_SCALE );
  gl_FragData[0] *= texture2D( oz_Textures[1], exTexCoord );
  gl_FragData[0] = applyFog( gl_FragData[0], dist );
}
