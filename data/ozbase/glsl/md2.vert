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
 * md2.vert
 *
 * Mesh shader that reads and interpolates vertex positions from the given vertex texture.
 */

#ifdef OZ_VERTEX_TEXTURE
uniform lowp vec3 oz_MD2Anim;
#endif

attribute lowp vec3 inPosition;
attribute lowp vec2 inTexCoord;
attribute lowp vec3 inNormal;

varying lowp vec3 exPosition;
varying lowp vec2 exTexCoord;
varying lowp vec3 exNormal;

void main()
{
#ifdef OZ_VERTEX_TEXTURE
  lowp vec4 firstPosition  = texture2D( oz_Textures[3], vec2( inPosition.x, oz_MD2Anim[0] ) );
  lowp vec4 secondPosition = texture2D( oz_Textures[3], vec2( inPosition.x, oz_MD2Anim[1] ) );
  lowp vec4 firstNormal    = texture2D( oz_Textures[4], vec2( inPosition.x, oz_MD2Anim[0] ) );
  lowp vec4 secondNormal   = texture2D( oz_Textures[4], vec2( inPosition.x, oz_MD2Anim[1] ) );
  lowp vec4 localPosition  = mix( firstPosition, secondPosition, oz_MD2Anim[2] );
  lowp vec4 localNormal    = mix( firstNormal, secondNormal, oz_MD2Anim[2] );

  gl_Position   = oz_Transform.complete * localPosition;
  exPosition    = ( oz_Transform.model * localPosition ).xyz;
  exTexCoord    = inTexCoord;
  exNormal      = ( oz_Transform.model * localNormal ).xyz;
#else
  gl_Position = oz_Transform.complete * vec4( inPosition, 1.0 );
  exPosition  = ( oz_Transform.model * vec4( inPosition, 1.0 ) ).xyz;
  exTexCoord  = inTexCoord;
  exNormal    = ( oz_Transform.model * vec4( inNormal, 0.0 ) ).xyz;
#endif
}
