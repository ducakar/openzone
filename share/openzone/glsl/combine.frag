/*
 *  combine.frag
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

struct CaelumLight
{
  vec3 dir;
  vec4 diffuse;
  vec4 ambient;
};

uniform CaelumLight oz_CaelumLight;

varying vec2 exTexCoord;

void main()
{
  vec4 colour = texture2D( oz_Textures[0], exTexCoord );
  vec3 normal = normalize( texture2D( oz_Textures[1], exTexCoord ).xyz );

  float diffuseFactor = max( 0.0, dot( -normal, oz_CaelumLight.dir ) );
  vec4 lighting = oz_CaelumLight.ambient + diffuseFactor * oz_CaelumLight.diffuse;

  gl_FragColor = colour * lighting;
}
