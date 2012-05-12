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

const lowp int   MS_LEVEL         = 8;
const lowp float MS_SIZE          = 0.0075;

const lowp float BLOOM_INTENSITY  = 1.5;
const lowp float BLOOM_LUMINANCE  = 0.3;

const lowp float MS_DIM           = float( MS_LEVEL - 1 ) / 2.0;
const lowp float MS_SPACING       = MS_SIZE / float( MS_LEVEL - 1 );
const lowp float MS_SAMPLES       = float( 2 * MS_LEVEL );

const lowp float BLUR_FACTOR      = 1.0 / MS_SAMPLES;
const lowp float BLOOM_FACTOR     = BLOOM_INTENSITY / MS_SAMPLES;
const lowp float LUMINANCE_FACTOR = BLOOM_LUMINANCE / MS_SAMPLES;

varying lowp vec2 exTexCoord;

void main()
{
  lowp vec4 multiSample = vec4( 0.0 );

  lowp float x = -MS_DIM;
  for( int i = 0; i < MS_LEVEL; ++i ) {
    lowp vec2 coords = vec2( exTexCoord.s + MS_SPACING * x, exTexCoord.t );
    lowp vec4 sample = texture2D( oz_Textures[0], coords );

    multiSample += sample;
    x += 1.0;
  }

  lowp float y = -MS_DIM;
  for( int i = 0; i < MS_LEVEL; ++i ) {
    lowp vec2 coords = vec2( exTexCoord.s, exTexCoord.t + MS_SPACING * y );
    lowp vec4 sample = texture2D( oz_Textures[0], coords );

    multiSample += sample;
    y += 1.0;
  }

  lowp vec4  blur      = multiSample * BLUR_FACTOR;
  lowp vec4  bloom     = multiSample * BLOOM_FACTOR;
  lowp vec4  sample    = texture2D( oz_Textures[0], exTexCoord );
  lowp float luminance = clamp( ( multiSample.r + multiSample.g + multiSample.b ) * LUMINANCE_FACTOR, 0.0, 1.0 );

//   gl_FragColor = mix( mix( sample, blur, 0 ), bloom, luminance );
  gl_FragColor = vec4( 1.0 );
}
