/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2012 Davorin Učakar
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @file client/Shader.cc
 *
 * Shader utilities.
 */

#include "stable.hh"

#include "client/Shader.hh"

#include "matrix/Library.hh"

#include "client/Camera.hh"
#include "client/OpenGL.hh"

#define OZ_REGISTER_PARAMETER( paramVar, uniformName ) \
  programs[id].param.paramVar = glGetUniformLocation( programs[id].program, uniformName )

#define OZ_REGISTER_ATTRIBUTE( location, name ) \
  glBindAttribLocation( programs[id].program, location, name )

namespace oz
{
namespace client
{

Param     param;
Transform tf;
Shader    shader;

char   Shader::logBuffer[LOG_BUFFER_SIZE];
String Shader::defines;

void Transform::ortho( int width, int height )
{
  float cx = float( 2.0 / double( width ) );
  float cy = float( 2.0 / double( height ) );

  proj = Mat44(    cx,  0.0f,  0.0f, 0.0f,
                 0.0f,    cy,  0.0f, 0.0f,
                 0.0f,  0.0f, -1.0f, 0.0f,
                -1.0f, -1.0f,  0.0f, 1.0f );
}

void Transform::projection()
{
  double lr   = double( client::camera.vertPlane );
  double tb   = double( client::camera.horizPlane );
  double near = double( client::Camera::MIN_DISTANCE );
  double far  = double( client::camera.maxDist );

  float cx = float( near / lr );
  float cy = float( near / tb );
  float cz = float( -( far + near ) / ( far - near ) );
  float tz = float( -( 2.0 * far * near ) / ( far - near ) );

  proj = Mat44(   cx, 0.0f, 0.0f,  0.0f,
                0.0f,   cy, 0.0f,  0.0f,
                0.0f, 0.0f,   cz, -1.0f,
                0.0f, 0.0f,   tz,  0.0f );
}

void Transform::applyCamera()
{
  projCamera = proj * camera;

  glUniform3fv( param.oz_CameraPosition, 1, client::camera.p );
}

void Transform::applyModel() const
{
  glUniformMatrix4fv( param.oz_Transform_model, 1, GL_FALSE, model );
}

void Transform::apply() const
{
  glUniformMatrix4fv( param.oz_Transform_model, 1, GL_FALSE, model );
  glUniformMatrix4fv( param.oz_Transform_complete, 1, GL_FALSE, projCamera * model );
}

const Shader::Light Shader::Light::NONE = Light( Point3::ORIGIN, Vec4::ZERO );

Shader::Light::Light( const Point3& pos_, const Vec4& diffuse_ ) :
  pos( pos_ ), diffuse( diffuse_ )
{}

void Shader::compileShader( uint id, const char* path, const char** sources, int* lengths ) const
{
  PhysFile file( path );
  if( !file.map() ) {
    throw Exception( "Shader source '%s' mmap failed", path );
  }

  InputStream is = file.inputStream();

  sources[2] = is.begin();
  lengths[2] = is.capacity();

  glShaderSource( id, 3, sources, lengths );
  glCompileShader( id );

  file.unmap();

  int result;
  glGetShaderiv( id, GL_COMPILE_STATUS, &result );

  int length;
  glGetShaderInfoLog( id, LOG_BUFFER_SIZE, &length, logBuffer );
  logBuffer[LOG_BUFFER_SIZE - 1] = '\0';

  if( length != 0 ) {
    if( result == GL_TRUE ) {
      log.verboseMode = true;
    }

    log.printRaw( "\n%s:\n%s", path, logBuffer );
    log.verboseMode = false;
  }

  if( result != GL_TRUE ) {
    throw Exception( "Shader '%s' compile failed", path );
  }

  OZ_GL_CHECK_ERROR();
}

void Shader::loadProgram( int id, const char** sources, int* lengths )
{
  const String& name = library.shaders[id].name;

  programs[id].vertShader = glCreateShader( GL_VERTEX_SHADER );
  programs[id].fragShader = glCreateShader( GL_FRAGMENT_SHADER );

  compileShader( programs[id].vertShader, "glsl/" + name + ".vert", sources, lengths );
  compileShader( programs[id].fragShader, "glsl/" + name + ".frag", sources, lengths );

  programs[id].program = glCreateProgram();
  glAttachShader( programs[id].program, programs[id].vertShader );
  glAttachShader( programs[id].program, programs[id].fragShader );

  OZ_REGISTER_ATTRIBUTE( Attrib::POSITION,            "inPosition" );
  OZ_REGISTER_ATTRIBUTE( Attrib::TEXCOORD,            "inTexCoord" );
  OZ_REGISTER_ATTRIBUTE( Attrib::DETAILCOORD,         "inDetailCoord" );
  OZ_REGISTER_ATTRIBUTE( Attrib::NORMAL,              "inNormal" );
  OZ_REGISTER_ATTRIBUTE( Attrib::TANGENT,             "inTangent" );
  OZ_REGISTER_ATTRIBUTE( Attrib::BINORMAL,            "inBinormal" );

  glLinkProgram( programs[id].program );

  int result;
  glGetProgramiv( programs[id].program, GL_LINK_STATUS, &result );

  int length;
  glGetProgramInfoLog( programs[id].program, LOG_BUFFER_SIZE, &length, logBuffer );
  logBuffer[LOG_BUFFER_SIZE - 1] = '\0';

  if( length != 0 ) {
    if( result == GL_TRUE ) {
      log.verboseMode = true;
    }

    log.printRaw( "\n%s:\n%s", name.cstr(), logBuffer );
    log.verboseMode = false;
  }

  if( result != GL_TRUE ) {
    throw Exception( "Shader program '%s' linking failed", name.cstr() );
  }

  glUseProgram( programs[id].program );

  OZ_REGISTER_PARAMETER( oz_Transform_model,          "oz_Transform.model" );
  OZ_REGISTER_PARAMETER( oz_Transform_complete,       "oz_Transform.complete" );

  OZ_REGISTER_PARAMETER( oz_CameraPosition,           "oz_CameraPosition" );

  OZ_REGISTER_PARAMETER( oz_Colour,                   "oz_Colour" );
  OZ_REGISTER_PARAMETER( oz_Textures,                 "oz_Textures" );

  OZ_REGISTER_PARAMETER( oz_CaelumLight_dir,          "oz_CaelumLight.dir" );
  OZ_REGISTER_PARAMETER( oz_CaelumLight_diffuse,      "oz_CaelumLight.diffuse" );
  OZ_REGISTER_PARAMETER( oz_CaelumLight_ambient,      "oz_CaelumLight.ambient" );

  OZ_REGISTER_PARAMETER( oz_Specular,                 "oz_Specular" );
  OZ_REGISTER_PARAMETER( oz_NightVision,              "oz_NightVision" );

  OZ_REGISTER_PARAMETER( oz_Fog_start,                "oz_Fog.start" );
  OZ_REGISTER_PARAMETER( oz_Fog_end,                  "oz_Fog.end" );
  OZ_REGISTER_PARAMETER( oz_Fog_colour,               "oz_Fog.colour" );

  OZ_REGISTER_PARAMETER( oz_WaveBias,                 "oz_WaveBias" );
  OZ_REGISTER_PARAMETER( oz_Wind,                     "oz_Wind" );
  OZ_REGISTER_PARAMETER( oz_MD2Anim,                  "oz_MD2Anim" );

  param = programs[id].param;

  if( config.getSet( "shader.setSamplerIndices", false ) ) {
    int textureIds[4] = { 0, 1, 2, 3 };
    glUniform1iv( param.oz_Textures, 4, textureIds );
  }

  OZ_GL_CHECK_ERROR();
}

Shader::Shader() :
  mode( UI ), plain( -1 ), defaultMasks( 0 )
{}

void Shader::use( int id )
{
  if( id == activeProgram ) {
    return;
  }

  activeProgram = id;

  glUseProgram( programs[id].program );
  param = programs[id].param;

  OZ_GL_CHECK_ERROR();
}

void Shader::push()
{
  programStack.pushLast( activeProgram );
}

void Shader::pop()
{
  use( programStack.popLast() );
}

void Shader::setLightingDistance( float distance )
{
  lightingDistance = distance;
}

void Shader::setAmbientLight( const Vec4& colour )
{
  caelumLight.ambient = colour;
}

void Shader::setCaelumLight( const Vec3& dir, const Vec4& colour )
{
  caelumLight.dir = dir;
  caelumLight.diffuse = colour;
}

void Shader::updateLights()
{
  glUniform3fv( param.oz_CaelumLight_dir,     1, caelumLight.dir );
  glUniform4fv( param.oz_CaelumLight_diffuse, 1, caelumLight.diffuse );
  glUniform4fv( param.oz_CaelumLight_ambient, 1, caelumLight.ambient );
}

void Shader::init()
{
  log.print( "Initialising Shader ..." );

  hasVertexTexture = config.getSet( "shader.vertexTexture", true );
  isDeferred       = config.get( "render.deferred", false );
  doPostprocess    = config.get( "render.postprocess", false );
  isLowDetail      = config.get( "render.lowDetail", false );

  // bind white texture to id 0 to emulate fixed functionality (in fixed functionality sampler
  // always returns white colour when texture 0 is bound)
  ubyte whitePixel[] = { 0xff, 0xff, 0xff, 0xff };

  glBindTexture( GL_TEXTURE_2D, 0 );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
  glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, whitePixel );

  // default masks (specular 1.0, emission 0.0)
  ubyte masksPixel[] = { 0xff, 0x00, 0x00, 0xff };

  glGenTextures( 1, &defaultMasks );
  glBindTexture( GL_TEXTURE_2D, defaultMasks );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
  glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, masksPixel );

  glBindTexture( GL_TEXTURE_2D, 0 );

  for( int i = 1; i >= 0; --i ) {
    glActiveTexture( GL_TEXTURE0 + uint( i ) );
    glBindTexture( GL_TEXTURE_2D, 0 );
  }

  programs.alloc( library.shaders.length() );
  for( int i = 0; i < library.shaders.length(); ++i ) {
    programs[i].program    = 0;
    programs[i].vertShader = 0;
    programs[i].fragShader = 0;
  }

  const char* sources[3];
  int         lengths[3];

  defines = "#version 120\n";
  defines = defines + ( hasVertexTexture ? "#define OZ_VERTEX_TEXTURE\n" : "\n" );
  defines = defines + ( isDeferred ?       "#define OZ_DEFERRED\n" : "\n" );
  defines = defines + ( doPostprocess ?    "#define OZ_POSTPROCESS\n" : "\n" );
  defines = defines + ( isLowDetail ?      "#define OZ_LOW_DETAIL\n" : "\n" );

  for( int i = 4; i < 10; ++i ) {
    defines = defines + "\n";
  }

  sources[0] = defines;
  lengths[0] = defines.length();

  Buffer buffer = PhysFile( "glsl/header.glsl" ).read();
  if( buffer.isEmpty() ) {
    throw Exception( "header.glsl reading failed" );
  }

  sources[1] = buffer.begin();
  lengths[1] = buffer.length();

  plain       = library.shaderIndex( "plain" );
  mesh        = library.shaderIndex( "mesh" );
  combine     = library.shaderIndex( "combine" );
  postprocess = library.shaderIndex( "postprocess" );

  colour = Vec4::ONE;
  medium = 0;

  for( int i = 0; i < library.shaders.length(); ++i ) {
    loadProgram( i, sources, lengths );
  }

  log.printEnd( " OK" );
}

void Shader::free()
{
  log.print( "Freeing Shader ..." );

  for( int i = 0; i < library.shaders.length(); ++i ) {
    if( programs[i].program != 0 ) {
      glDetachShader( programs[i].program, programs[i].vertShader );
      glDetachShader( programs[i].program, programs[i].fragShader );
      glDeleteProgram( programs[i].program );
      programs[i].program = 0;
    }
    if( programs[i].vertShader != 0 ) {
      glDeleteShader( programs[i].vertShader );
      programs[i].vertShader = 0;
    }
    if( programs[i].fragShader != 0 ) {
      glDeleteShader( programs[i].fragShader );
      programs[i].fragShader = 0;
    }
  }

  if( defaultMasks != 0 ) {
    glDeleteTextures( 1, &defaultMasks );
    defaultMasks = 0;
  }

  plain = -1;
  defines = "";

  programs.dealloc();

  OZ_GL_CHECK_ERROR();

  log.printEnd( " OK" );
}

}
}
