/*
 *  bigTerraLand.vert
 *
 *  Terrain (land) shader.
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

in vec3 inPosition;
in vec2 inTexCoord;
in vec3 inNormal;

out vec2  exTexCoord;
out vec4  exColour;
out float exDistance;

void main()
{
  vec3 position = ( oz_Transform.model * vec4( inPosition, 1.0 ) ).xyz;
  vec3 normal   = ( oz_Transform.model * vec4( inNormal, 0.0 ) ).xyz;
  vec3 toCamera = oz_CameraPosition - position;

  exTexCoord    = inTexCoord;
  exColour      = skyLightColour( normal );
  exDistance    = length( toCamera );
  gl_Position   = oz_Transform.complete * vec4( inPosition, 1.0 );
}
