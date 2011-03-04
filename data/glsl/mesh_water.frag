/*
 *  mesh_near.frag
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

varying vec4 position;
varying vec2 texCoord;
varying vec3 skyLightColour;

void main()
{
  float fogRatio = min( length( position ) / oz_FogDistance, 1.0 );

  gl_FragColor = texture2D( oz_Textures[0], texCoord * oz_TextureScales[0] );
  gl_FragColor.xyz *= skyLightColour;
  gl_FragColor = mix( gl_FragColor, oz_FogColour, fogRatio );

  gl_FragColor.x *= 2.0;

  if( oz_IsHighlightEnabled ) {
    float avgColour = ( gl_FragColor.x + gl_FragColor.y + gl_FragColor.z ) / 3.0;
    vec3  highlightColour = oz_HighlightBase + oz_HighlightFactor * avgColour;
    gl_FragColor.xyz = mix( gl_FragColor.xyz, highlightColour, 0.5 );
  }
}
