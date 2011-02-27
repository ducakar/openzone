/*
 *  default.frag
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

varying vec3 oz_TransformedPointLights[16];

varying vec3 oz_Position;
varying vec3 oz_Normal;

void main()
{
  vec4  texel0  = texture2D( oz_Textures[0], gl_TexCoord[0].st * oz_TextureScale );
  vec4  texel1  = texture2D( oz_Textures[0], gl_TexCoord[0].st * oz_TextureScale );
  vec4  texel   = texel0 * oz_DiffuseMaterial;

  vec3 diffuse = oz_SkyLight[1] * dot( normalize( oz_Normal ), oz_SkyLight[0] );
  diffuse = clamp( diffuse, vec3( 0.0, 0.0, 0.0 ), vec3( 1.0, 1.0, 1.0 ) );

//   vec3 light0Pos = oz_TransformedPointLights[0] - oz_Position;
//   float light0Intensity = min( 50.0 / sqrt( dot( light0Pos, light0Pos ) ), 1.0 );
//
  vec3 light = oz_AmbientLight + diffuse;// + light0Intensity;
  light = clamp( light, vec3( 0.0, 0.0, 0.0 ), vec3( 1.0, 1.0, 1.0 ) );

  gl_FragColor = vec4( gl_Color.xyz + texel.xyz, gl_Color.a * texel.a );
  gl_FragColor.xyz *= light;
}
