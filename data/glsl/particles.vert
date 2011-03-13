/*
 *  particles.vert
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

in vec3 inPosition;
in vec2 inTexCoord;
in vec3 inNormal;

out vec4 exColour;

void main()
{
  vec4  position = oz_Transform.model * vec4( inPosition, 1.0 );
  vec3  normal   = ( oz_Transform.model * vec4( inNormal, 0.0 ) ).xyz;
  float dist     = length( position );

  exColour       = oz_Colour;
  exColour       *= skyLightColour( normal );
  exColour       = applyFog( exColour, dist );

  gl_Position    = oz_Transform.complete * vec4( inPosition, 1.0 );
}
