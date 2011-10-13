/*
 *  terraWater.frag
 *
 *  Terrain water (sea) shader.
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

const float TERRA_WATER_SCALE = 512.0;

varying vec2 exTexCoord;

void main()
{
  gl_FragData[0] = texture2D( oz_Textures[0], exTexCoord * TERRA_WATER_SCALE );
  gl_FragData[1] = vec4( 0.0, 0.0, 1.0, 0.0 );
}
