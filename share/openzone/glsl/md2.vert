/*
 *  md2.vert
 *
 *  Mesh shader that reads in interpolates vertex positions from the given vertex texture.
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

in vec3 inPosition;
in vec2 inTexCoord;
in vec3 inNormal;

out vec3 exPosition;
out vec2 exTexCoord;
out vec3 exNormal;

void main()
{
#ifdef OZ_VERTEX_TEXTURE
  vec4 firstPosition  = texture( oz_Textures[1], vec2( inPosition.x, oz_MD2Anim[0] ) );
  vec4 secondPosition = texture( oz_Textures[1], vec2( inPosition.x, oz_MD2Anim[1] ) );
  vec4 firstNormal    = texture( oz_Textures[2], vec2( inPosition.x, oz_MD2Anim[0] ) );
  vec4 secondNormal   = texture( oz_Textures[2], vec2( inPosition.x, oz_MD2Anim[1] ) );
  vec4 localPosition  = mix( firstPosition, secondPosition, oz_MD2Anim[2] );
  vec4 localNormal    = mix( firstNormal, secondNormal, oz_MD2Anim[2] );

  exPosition    = ( oz_Transform.model * localPosition ).xyz;
  exTexCoord    = inTexCoord;
  exNormal      = ( oz_Transform.model * localNormal ).xyz;
  gl_Position   = oz_Transform.complete * localPosition;
#else
  exPosition  = ( oz_Transform.model * vec4( inPosition, 1.0 ) ).xyz;
  exTexCoord  = inTexCoord;
  exNormal    = ( oz_Transform.model * vec4( inNormal, 0.0 ) ).xyz;
  gl_Position = oz_Transform.complete * vec4( inPosition, 1.0 );
#endif
}
