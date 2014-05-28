/*
 * ozEngine - OpenZone Engine Library.
 *
 * Copyright © 2002-2014 Davorin Učakar
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from
 * the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software in
 *    a product, an acknowledgement in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

/**
 * @file ozEngine/Shaders.cc
 */

#include "Shaders.hh"

#if 0
#include "GL.hh"

namespace oz
{

enum Type
{
  POSITION,
  TEXCOORD,
  NORMAL
};

struct Uniforms
{
  int projModelTransform;
  int modelTransform;
  int boneTransforms;
  int meshAnimation;

  int colourTransform;
  int textures;

  int caelumLight_dir;
  int caelumLight_diffuse;
  int caelumLight_ambient;
  int cameraPosition;

  int fog_dist;
  int fog_colour;

  int starsColour;
  int waveBias;
  int wind;
};

struct Program
{
  GLuint   vertShader;
  GLuint   fragShader;
  GLuint   program;
  Uniforms uniforms;
};

static List<Program>    programs;
static Map<String, int> programIndices;

int Shaders::load( const char* path_ )
{
  const int* index = programIndices.find( path_ );
  if( index != nullptr ) {
    return *index;
  }

  programs.add();
  Program& p = programs.last();

  String path = path_;
  File   configFile = path + ".json";
  JSON   programConfig;

  if( !programConfig.load( configFile ) ) {
    return -1;
  }

  p.vertShader = glCreateShader( GL_VERTEX_SHADER );
  p.fragShader = glCreateShader( GL_FRAGMENT_SHADER );
  p.program    = glCreateProgram();

  String vertPath = programConfig[0].asString() + ".vert";
  String fragPath = programConfig[1].asString() + ".frag";
  String dirName  = configFile.directory();

  if( !dirName.isEmpty() ) {
    vertPath = dirName + "/" + vertPath;
    fragPath = dirName + "/" + fragPath;
  }

  if( !GL::compileShaderFromFile( p.vertShader, "", File( vertPath ) ) ||
      !GL::compileShaderFromFile( p.vertShader, "", File( fragPath ) ) )
  {
    glDeleteShader( p.vertShader );
    glDeleteShader( p.fragShader );
    glDeleteProgram( p.program );
    return -1;
  }

  glAttachShader( p.program, p.vertShader );
  glAttachShader( p.program, p.fragShader );
  glLinkProgram( p.program );

  int result;
  glGetProgramiv( p.program, GL_LINK_STATUS, &result );

  if( result != GL_TRUE ) {
    glDetachShader( p.program, p.vertShader );
    glDetachShader( p.program, p.fragShader );
    glDeleteShader( p.vertShader );
    glDeleteShader( p.fragShader );
    glDeleteProgram( p.program );
    return -1;
  }

  glBindAttribLocation( p.program, POSITION, "inPosition" );
  glBindAttribLocation( p.program, TEXCOORD, "inTexCoord" );
  glBindAttribLocation( p.program, NORMAL, "inNormal" );

  p.uniforms.projModelTransform  = glGetUniformLocation( p.program, "oz_ProjModelTransform" );
  p.uniforms.modelTransform      = glGetUniformLocation( p.program, "oz_ModelTransform" );
  p.uniforms.boneTransforms      = glGetUniformLocation( p.program, "oz_BoneTransforms" );
  p.uniforms.colourTransform     = glGetUniformLocation( p.program, "oz_ColourTransform" );

  p.uniforms.cameraPosition      = glGetUniformLocation( p.program, "oz_CameraPosition" );
  p.uniforms.meshAnimation       = glGetUniformLocation( p.program, "oz_MeshAnimation" );
  p.uniforms.textures            = glGetUniformLocation( p.program, "oz_Textures" );

  p.uniforms.caelumLight_dir     = glGetUniformLocation( p.program, "oz_CaelumLight.dir" );
  p.uniforms.caelumLight_diffuse = glGetUniformLocation( p.program, "oz_CaelumLight.diffuse" );
  p.uniforms.caelumLight_ambient = glGetUniformLocation( p.program, "oz_CaelumLight.ambient" );

  p.uniforms.fog_dist            = glGetUniformLocation( p.program, "oz_Fog.dist" );
  p.uniforms.fog_colour          = glGetUniformLocation( p.program, "oz_Fog.colour" );

  p.uniforms.starsColour         = glGetUniformLocation( p.program, "oz_StarsColour" );
  p.uniforms.waveBias            = glGetUniformLocation( p.program, "oz_WaveBias" );
  p.uniforms.wind                = glGetUniformLocation( p.program, "oz_Wind" );

  OZ_GL_CHECK_ERROR();
  return programs.length() - 1;
}

void Shaders::destroy()
{
  for( const Program& p : programs ) {
    glDetachShader( p.program, p.vertShader );
    glDetachShader( p.program, p.fragShader );
    glDeleteShader( p.vertShader );
    glDeleteShader( p.fragShader );
    glDeleteProgram( p.program );
  }

  programIndices.clear();
  programIndices.deallocate();
  programs.clear();
  programs.deallocate();
}

const int Shaders::oz_colourMatrix = -1;

bool Shaders::init()
{
  *const_cast<int*>( &oz_colourMatrix ) = 0;
  return true;
}

}
#endif
