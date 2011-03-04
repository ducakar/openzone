/*
 *  mesh_itemview.frag
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

varying vec2 oz_FragTexCoord;

void main()
{
  gl_FragColor = texture2D( oz_Textures[0], oz_FragTexCoord * oz_TextureScales[0] );

  if( oz_IsHighlightEnabled ) {
    float avgColour = ( gl_FragColor.x + gl_FragColor.y + gl_FragColor.z ) / 3.0;
    vec3  highlightColour = oz_HighlightBase + oz_HighlightFactor * avgColour;
    gl_FragColor.xyz = mix( gl_FragColor.xyz, highlightColour, 0.5 );
  }
}
