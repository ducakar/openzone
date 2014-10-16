/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2014 Davorin Učakar
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

precision highp float;

const int   MS_LEVEL        = 8;
const float MS_SIZE         = 0.02;
const float MS_DIM          = float(MS_LEVEL - 1) / 2.0;
const float MS_SPACING      = MS_SIZE / float(MS_LEVEL - 1);
const float BLOOM_THRESHOLD = 0.3;
const float BLOOM_INTENSITY = 0.8 / float(MS_LEVEL);

uniform mat4      oz_Colour;
uniform sampler2D oz_Texture;
uniform sampler2D oz_Masks;

varying vec2 exTexCoord;

void main()
{
  float multiSample = 0.0;

  for(float x = -MS_DIM; x <= MS_DIM; x += 1.0) {
    vec2  coords    = vec2(exTexCoord.s + MS_SPACING * x, exTexCoord.t);
    vec4  specular  = texture2D(oz_Masks, coords);
    float luminance = specular.r + specular.g + specular.b;

    multiSample += max(0.0, luminance - BLOOM_THRESHOLD);
  }

  vec4 colour = texture2D(oz_Texture, exTexCoord);
  vec3 bloom  = vec3(multiSample * BLOOM_INTENSITY);

  gl_FragColor = colour + vec4(bloom, 0);
}
