/*
 *  bigTerraLand.frag
 *
 *  Terrain (land) shader.
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

const float TERRA_DETAIL_SCALE = 512.0;

varying vec2 exTexCoord;
varying vec4 exNormal;

void main()
{
  vec4 detailTexel = texture2D( oz_Textures[0], exTexCoord * TERRA_DETAIL_SCALE );
  vec4 mapTexel    = texture2D( oz_Textures[1], exTexCoord );

  gl_FragData[0] = detailTexel * mapTexel;
  gl_FragData[1] = exNormal;
}
