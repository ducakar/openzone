/*
 * OpenZone - Simple Cross-Platform FPS/RTS Game Engine
 * Copyright (C) 2002-2011  Davorin Učakar
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Davorin Učakar
 * <davorin.ucakar@gmail.com>
 */

/*
 * terraWater.frag
 *
 * Terrain sea surface shader.
 */

const float TERRA_WATER_SCALE = 512.0;
const vec3  NORMAL            = vec3( 0.0, 0.0, 1.0 );

varying vec3 exPosition;
varying vec2 exTexCoord;

void main()
{
  vec3 toCamera = oz_CameraPosition - exPosition;
  float dist    = length( toCamera );

  vec4 colourSample   = texture2D( oz_Textures[0], exTexCoord * TERRA_WATER_SCALE );
  vec4 specularSample = texture2D( oz_Textures[1], exTexCoord * TERRA_WATER_SCALE );

  vec4 diffuse  = skyLightColour( NORMAL );
  vec4 specular = specularColour( oz_Specular, NORMAL, toCamera / dist );

  gl_FragData[0] = vec4( colourSample.xyz, 0.75 ) * ( diffuse + specular );
  gl_FragData[0] = applyFog( gl_FragData[0], dist );
}
