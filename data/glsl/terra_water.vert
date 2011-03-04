/*
 *  terra_near.vert
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

attribute vec3 oz_Position;
attribute vec2 oz_TexCoord;
attribute vec3 oz_Normal;
// attribute vec3 oz_Tangent;
// attribute vec3 oz_Binormal;
// attribute vec2 oz_TexCoord[2];

varying vec3 oz_CameraPosition;
varying vec3 oz_CameraNormal;
// varying vec3 oz_CameraTangent;
// varying vec3 oz_CameraBinormal;
// varying vec2 oz_FragTexCoord[2];
varying vec2 oz_FragTexCoord;

varying vec3 oz_SkyLightColour;
// varying vec3 oz_TransformedPointLights[16];

void main()
{
  gl_Position        = gl_ModelViewProjectionMatrix * vec4( oz_Position, 1.0 );

  oz_CameraPosition  = gl_ModelViewMatrix * vec4( oz_Position, 1.0 );
  oz_CameraNormal    = gl_NormalMatrix    * oz_Normal;
//   oz_CameraTangent   = gl_NormalMatrix    * oz_Tangent;
//   oz_CameraBinormal  = gl_NormalMatrix    * oz_Binormal;

//   oz_FragTexCoord[0] = oz_TexCoord[0];
//   oz_FragTexCoord[1] = oz_TexCoord[1];
  oz_FragTexCoord    = oz_TexCoord;

  float diffuseFactor = clamp( dot( oz_SkyLight[0], oz_CameraNormal ), 0.0, 1.0 );
  oz_SkyLightColour = diffuseFactor * oz_SkyLight[1] + oz_SkyLight[2];

//   oz_TransformedPointLights[0] = ( gl_ModelViewMatrix * vec4( oz_PointLights[0] ) ).xyz;
}
