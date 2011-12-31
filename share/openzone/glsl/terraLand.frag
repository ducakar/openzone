/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2012 Davorin Učakar
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
  vec3  toCamera = oz_CameraPosition - exPosition;
  vec3  normal   = normalize( exNormal );
  float dist     = length( toCamera );

  vec4 detailSample = texture2D( oz_Textures[0], exTexCoord * TERRA_DETAIL_SCALE );
  vec4 mapSample    = texture2D( oz_Textures[2], exTexCoord );

  vec4 diffuse = skyLightColour( normal );

  gl_FragData[0] = detailSample * mapSample * diffuse;
  gl_FragData[0] = applyFog( gl_FragData[0], dist );
}
