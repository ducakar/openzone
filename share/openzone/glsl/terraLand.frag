/*
 *  bigTerraLand.frag
 *
 *  Terrain (land) shader.
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

varying vec2  exTexCoord;
varying vec3  exNormal;
varying float exDistance;

void main()
{
  gl_FragData[0] = skyLightColour( normalize( exNormal ) );
  gl_FragData[0] *= texture2D( oz_Textures[0], exTexCoord * TERRA_DETAIL_SCALE );
  gl_FragData[0] *= texture2D( oz_Textures[1], exTexCoord );
  gl_FragData[0] = applyFog( gl_FragData[0], exDistance );
}
