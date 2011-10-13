/*
 *  terraWater.frag
 *
 *  Terrain water (sea) shader.
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

const float TERRA_WATER_SCALE = 512.0;
const vec3  NORMAL            = vec3( 0.0, 0.0, 1.0 );

varying vec3 exPosition;
varying vec2 exTexCoord;

void main()
{
  vec3 toCamera = oz_CameraPosition - exPosition;
  float dist = length( toCamera );

  gl_FragData[0] = vec4( 1.0, 1.0, 1.0, 0.75 );
  gl_FragData[0] *= skyLightColour( NORMAL );
  gl_FragData[0] *= specularColour( NORMAL, toCamera / dist );
  gl_FragData[0] *= texture2D( oz_Textures[0], exTexCoord * TERRA_WATER_SCALE );
  gl_FragData[0] = applyFog( gl_FragData[0], dist );
}
