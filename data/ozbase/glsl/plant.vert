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
 * plant.vert
 *
 * Mesh shader that deforms mesh according to the given wind.
 */

uniform vec4 oz_Wind;

attribute vec3 inPosition;
attribute vec2 inTexCoord;
attribute vec3 inNormal;

varying vec3 exPosition;
varying vec2 exTexCoord;
varying vec3 exNormal;

void main()
{
  exPosition     = ( oz_ModelTransform * vec4( inPosition, 1.0 ) ).xyz;
  exTexCoord     = inTexCoord;
  exNormal       = ( oz_ModelTransform * vec4( inNormal, 0.0 ) ).xyz;

  float windFact = max( inPosition.z, 0.0 );
  vec2  windBias = oz_Wind.xy * windFact*windFact * oz_Wind.z *
                        sin( 0.08 * ( exPosition.x + exPosition.y ) + oz_Wind.w );

  gl_Position    = oz_ProjModelTransform * vec4( inPosition.xy + windBias.xy, inPosition.z, 1.0 );
}
