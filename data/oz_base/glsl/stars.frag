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
 * stars.frag
 */

#include "header.glsl"

uniform vec4  oz_StarsColour;

varying vec3  exPosition;
varying float exAzimuth;

void main()
{
  float dist        = ( length( exPosition ) - 5.0 ) / 5.0;
  vec4  diff        = oz_StarsColour - oz_Fog.colour;
  vec4  disturbance = vec4( sin( 256.0 * exPosition ) * 0.10, 0.0 ) * dot( diff, diff );
  vec4  colour      = oz_StarsColour + disturbance;

  gl_FragData[0]    = oz_ColourTransform * mix( colour, oz_Fog.colour, dist );
}
