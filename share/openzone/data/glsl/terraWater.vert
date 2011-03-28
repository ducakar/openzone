/*
 *  bigTerraWater.vert
 *
 *  Terrain water (sea) shader.
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

in vec3 inPosition;
in vec2 inTexCoord;
in vec3 inNormal;

out vec3  exPosition;
out vec2  exTexCoord;
out float exDistance;
out vec4  exColour;

void main()
{
  float z = 0.15 * sin( oz_WaveBias + inPosition.x + inPosition.y );
  vec4 localPos = vec4( inPosition.x, inPosition.y, z, 1.0 );
  vec3 position = ( oz_Transform.model * localPos ).xyz;
  vec3 normal   = vec3( 0.0, 0.0, 1.0 );
  vec3 toCamera = oz_CameraPosition - position;

  exPosition    = position;
  exTexCoord    = inTexCoord;
  exDistance    = length( toCamera );
  exColour      = vec4( 1.0, 1.0, 1.0, 0.75 );
  exColour      *= skyLightColour( normal );
  exColour      *= specularColour( normal, toCamera / exDistance );
  gl_Position   = oz_Transform.complete * localPos;
}
