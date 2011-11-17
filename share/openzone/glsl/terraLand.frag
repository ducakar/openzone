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
 * terraLand.frag
 *
 * Terrain (land) shader.
 */

const float TERRA_DETAIL_SCALE = 512.0;

varying vec3 exPosition;
varying vec2 exTexCoord;
varying vec3 exNormal;

void main()
{
  vec3 toCamera = oz_CameraPosition - exPosition;
  vec3 normal   = normalize( exNormal );
  float dist    = length( toCamera );

  vec4 colour   = texture2D( oz_Textures[0], exTexCoord * TERRA_DETAIL_SCALE );
  vec4 map      = texture2D( oz_Textures[2], exTexCoord );

  gl_FragData[0] = colour * map;
  gl_FragData[0] *= skyLightColour( normal );
  gl_FragData[0] = applyFog( gl_FragData[0], dist );
}
