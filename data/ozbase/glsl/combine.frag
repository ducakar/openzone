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
 * combine.frag
 *
 * Lighting pass for deferred shading.
 */

varying lowp vec2 exTexCoord;

void main()
{
  lowp vec4 colour = texture2D( oz_Textures[0], exTexCoord );
  lowp vec3 normal = normalize( texture2D( oz_Textures[1], exTexCoord ).xyz );

  lowp float diffuseFactor = max( 0.0, dot( -normal, oz_CaelumLight.dir ) );
  lowp vec4 lighting = oz_CaelumLight.ambient + diffuseFactor * oz_CaelumLight.diffuse;

  gl_FragColor = colour * lighting;
}
