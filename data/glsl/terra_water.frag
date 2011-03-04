/*
 *  terra_near.frag
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

varying vec3 oz_CameraPosition;
// varying vec3 oz_CameraNormal;
// varying vec3 oz_CameraTangent;
// varying vec3 oz_CameraBinormal;
// varying vec2 oz_FragTexCoord[2];
varying vec2 oz_FragTexCoord;

varying vec3 oz_SkyLightColour;
// varying vec3 oz_TransformedPointLights[16];

void main()
{
  float dist = length( oz_CameraPosition );
  if( dist > oz_FogDistance ) {
    discard;
  }

  gl_FragColor  = texture2D( oz_Textures[0], oz_FragTexCoord * oz_TextureScales[0] );
  gl_FragColor *= texture2D( oz_Textures[1], oz_FragTexCoord * oz_TextureScales[1] );

  vec3 light = oz_SkyLightColour;// * vec3( oz_DiffuseMaterial );
  light = clamp( light, vec3( 0.0, 0.0, 0.0 ), vec3( 1.0, 1.0, 1.0 ) );

  gl_FragColor.xyz *= light;
  gl_FragColor = mix( gl_FragColor, oz_FogColour, dist / oz_FogDistance );

  if( oz_IsHighlightEnabled ) {
    float avgColour = ( gl_FragColor.x + gl_FragColor.y + gl_FragColor.z ) / 3.0;
    vec3  highlightColour = oz_HighlightBase + oz_HighlightFactor * avgColour;
    gl_FragColor.xyz = mix( gl_FragColor.xyz, highlightColour, 0.5 );
  }
}
