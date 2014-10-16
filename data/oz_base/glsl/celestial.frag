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
 * celestial.frag
 *
 * Shader for skybox and celestial bodies.
 */

precision highp float;

uniform mat4      oz_Colour;
uniform vec3      oz_CaelumColour;
uniform float     oz_CaelumLuminance;
uniform sampler2D oz_Texture;

varying vec2  exTexCoord;
varying float exAzimuth;

void main()
{
  vec4  texel   = texture2D(oz_Texture, exTexCoord);
  float azimuth = clamp(exAzimuth, 0.0, 1.0);
  vec3  colour  = oz_CaelumColour + (azimuth * oz_CaelumLuminance) * texel.xyz;

  gl_FragData[0] = oz_Colour * vec4(colour, texel.w);
#ifdef OZ_POSTPROCESS
  gl_FragData[1] = vec4(0.0, 0.0, 0.0, 1.0);
#endif
}
