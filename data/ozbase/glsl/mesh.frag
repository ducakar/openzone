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
 * mesh.frag
 *
 * Generic shader for meshes.
 */

varying lowp vec3 exPosition;
varying lowp vec2 exTexCoord;
varying lowp vec3 exNormal;

void main()
{
  lowp vec3  normal      = normalize( exNormal );

#ifdef OZ_LOW_DETAIL
  lowp float dist        = gl_FragCoord.w / gl_FragCoord.z;
#else
  lowp vec3  toCamera    = oz_CameraPosition - exPosition;
  lowp float dist        = length( toCamera );
#endif

  lowp vec4 colourSample = texture2D( oz_Textures[0], exTexCoord );
  lowp vec4 diffuse      = skyLightColour( normal );

#ifdef OZ_LOW_DETAIL
  lowp vec4 fragColour   = oz_Colour * colourSample * diffuse;
#else
  lowp vec4 masksSample  = texture2D( oz_Textures[1], exTexCoord );

  lowp vec4 emission     = vec4( masksSample.g, masksSample.g, masksSample.g, 0.0 );
  lowp vec4 specular     = specularColour( masksSample.r, normal, toCamera / dist );

  lowp vec4 fragColour   = oz_Colour * colourSample * ( diffuse + emission + specular );
#endif

  if( oz_NightVision ) {
    lowp float nvColour = 2.0 * ( fragColour.r + fragColour.g + fragColour.b );

    gl_FragData[0] = applyFog( vec4( 0.0, nvColour, 0.0, fragColour.a ), dist );
  }
  else {
    gl_FragData[0] = applyFog( fragColour, dist );
  }
}
