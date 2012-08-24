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
 * postprocess.frag
 *
 * Postprocess pass.
 */

const int   MS_LEVEL         = 16;
const float MS_SIZE          = 0.04;

const float BLOOM_THRESHOLD  = 2.0;
const float BLOOM_INTENSITY  = 0.5 / float( MS_LEVEL );

const float MS_DIM           = float( MS_LEVEL - 1 ) / 2.0;
const float MS_SPACING       = MS_SIZE / float( MS_LEVEL - 1 );

varying vec2 exTexCoord;

void main()
{
  vec3 multiSample = vec3( 0.0 );

  for( float x = -MS_DIM; x <= MS_DIM; x += 1.0 ) {
    vec2  coords    = vec2( exTexCoord.s + MS_SPACING * x, exTexCoord.t );
    vec4  sample    = texture2D( oz_Textures[0], coords );
    float luminance = sample.r + sample.g + sample.b;

    multiSample += vec3( max( luminance - BLOOM_THRESHOLD, 0.0 ) );
  }

  vec4 sample = texture2D( oz_Textures[0], exTexCoord );
  vec3 bloom  = multiSample * BLOOM_INTENSITY;

  gl_FragColor = sample + vec4( bloom, 0.0 );
}
