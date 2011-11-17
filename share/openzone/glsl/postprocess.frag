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
 * Davorin Učakar <davorin.ucakar@gmail.com>
 */

/*
 * postprocess.frag
 *
 * Postprocess pass.
 */

const float BLOOM_SIZE      = 2.0;
const float BLOOM_SPACING   = 0.0012;
const float BLOOM_THRESHOLD = 2.0;
const float BLOOM_FACTOR    = 0.02;

varying vec2 exTexCoord;

void main()
{
  // bloom
  float bloom = 0.0;

  for( float x = -BLOOM_SIZE; x <= BLOOM_SIZE; ++x ) {
    vec2  coords    = vec2( exTexCoord.s + BLOOM_SPACING * x, exTexCoord.t );
    vec4  sample    = texture2D( oz_Textures[0], coords );
    float luminance = sample.r + sample.g + sample.b;

    if( luminance > BLOOM_THRESHOLD ) {
      bloom += luminance - BLOOM_THRESHOLD;
    }
  }
  for( float y = -BLOOM_SIZE; y <= BLOOM_SIZE; ++y ) {
    vec2  coords    = vec2( exTexCoord.s, exTexCoord.t + BLOOM_SPACING * y );
    vec4  sample    = texture2D( oz_Textures[0], coords );
    float luminance = sample.r + sample.g + sample.b;

    if( luminance > BLOOM_THRESHOLD ) {
      bloom += luminance - BLOOM_THRESHOLD;
    }
  }

  gl_FragColor = texture2D( oz_Textures[0], exTexCoord );
  if( bloom > 1 ) {
    gl_FragColor += BLOOM_FACTOR * bloom;
  }

}
