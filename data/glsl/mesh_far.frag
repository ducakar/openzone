/*
 *  mesh_far.frag
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

varying vec2 texCoord;
varying float fogRatio;
varying vec3 skyLightColour;

void main()
{
  gl_FragColor = texture2D( oz_Textures[0], texCoord * oz_TextureScales[0] );
  gl_FragColor.xyz *= skyLightColour;
  gl_FragColor = mix( gl_FragColor, oz_FogColour, fogRatio );
}
