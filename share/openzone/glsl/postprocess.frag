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
 * postprocess.frag
 *
 * Postprocess pass.
 */

const float BLOOM_SIZE      = 4.0;
const float BLOOM_SPACING   = 0.0012;

varying vec2 exTexCoord;

void main()
{
  // bloom
  vec4 bloom = vec4( 0.0 );

  for( float x = -BLOOM_SIZE; x <= BLOOM_SIZE; ++x ) {
    vec2 coords = vec2( exTexCoord.s + BLOOM_SPACING * x, exTexCoord.t );
    vec4 sample = texture2D( oz_Textures[1], coords );

    bloom += sample;
  }
  for( float y = -BLOOM_SIZE; y <= BLOOM_SIZE; ++y ) {
    vec2 coords = vec2( exTexCoord.s, exTexCoord.t + BLOOM_SPACING * y );
    vec4 sample = texture2D( oz_Textures[1], coords );

    bloom += sample;
  }

  vec4 sample = texture2D( oz_Textures[0], exTexCoord );

  gl_FragColor = mix( sample, bloom * 0.15, 0.15 );
}
