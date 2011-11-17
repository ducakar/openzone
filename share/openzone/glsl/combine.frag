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
 * combine.frag
 *
 * Lighting pass for deferred shading.
 */

varying vec2 exTexCoord;

void main()
{
  vec4 colour = texture2D( oz_Textures[0], exTexCoord );
  vec3 normal = normalize( texture2D( oz_Textures[1], exTexCoord ).xyz );

  float diffuseFactor = max( 0.0, dot( -normal, oz_CaelumLight.dir ) );
  vec4 lighting = oz_CaelumLight.ambient + diffuseFactor * oz_CaelumLight.diffuse;

  gl_FragColor = colour * lighting;
}
