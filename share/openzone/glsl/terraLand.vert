/*
 *  bigTerraLand.vert
 *
 *  Terrain (land) shader.
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

attribute vec3 inPosition;
attribute vec2 inTexCoord;
attribute vec3 inNormal;

varying vec2 exTexCoord;
varying vec4 exNormal;

void main()
{
  gl_Position = oz_Transform.complete * vec4( inPosition, 1.0 );
  exTexCoord  = inTexCoord;
  exNormal    = oz_Transform.complete * vec4( inNormal, 0.0 );
}
