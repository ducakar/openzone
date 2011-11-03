/*
 * OpenZone - Simple Cross-Platform FPS/RTS Game Engine
 * Copyright (C) 2002-2011  Davorin Učakar
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
 *
 * Davorin Učakar <davorin.ucakar@gmail.com>
 */

/**
 * @file client/Shader.cpp
 *
 * Shader utilities.
 */

#include "stable.hpp"

#include "client/Shader.hpp"

#include "client/Camera.hpp"
#include "client/OpenGL.hpp"

#define OZ_REGISTER_PARAMETER( paramVar, uniformName ) \
  programs[id].param.paramVar = glGetUniformLocation( programs[id].program, uniformName )

#define OZ_REGISTER_ATTRIBUTE( location, name ) \
  glBindAttribLocation( programs[id].program, location, name )

#define OZ_REGISTER_FRAGDATA( location, name ) \
  glBindFragDataLocation( programs[id].program, location, name )

namespace oz
{
namespace client
{

Param     param;
Transform tf;
Shader    shader;

String Shader::defines;

void Transform::ortho()
{
  float cx = float( 2.0 / double( client::camera.width ) );
  float cy = float( 2.0 / double( client::camera.height ) );

  proj = Mat44(    cx,  0.0f,  0.0f, 0.0f,
                 0.0f,    cy,  0.0f, 0.0f,
                 0.0f,  0.0f, -1.0f, 0.0f,
                -1.0f, -1.0f,  0.0f, 1.0f );
}

void Transform::projection()
{
  double lr   = double( client::camera.vertPlane );
  double tb   = double( client::camera.horizPlane );
  double near = double( client::camera.minDist );
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
  log.print( "Compiling '%s' ...", path );

  File file( path );
  if( !file.map() ) {
    throw Exception( "Shader source mmap failed" );
  }

  InputStream is = file.inputStream();

  sources[2] = is.begin();
  lengths[2] = is.capacity();

  glShaderSource( id, 3, sources, lengths );
  glCompileShader( id );

  file.unmap();

  int result;
  glGetShaderiv( id, GL_COMPILE_STATUS, &result );

  DArray<char> logBuffer( BUFFER_SIZE );
  int length;

  glGetShaderInfoLog( id, BUFFER_SIZE, &length, logBuffer );
  logBuffer[BUFFER_SIZE - 1] = '\0';

  if( result != GL_TRUE ) {
    log.printEnd( " Failed:" );
    log.printRaw( "%s", &logBuffer[0] );
  }
  else if( length != 0 ) {
    log.printEnd( " OK, but:" );
    log.printRaw( "%s\n", &logBuffer[0] );
  }
  else {
    log.printEnd( " OK" );
  }

  if( result != GL_TRUE ) {
    throw Exception( "Shader compile failed" );
  }

  OZ_GL_CHECK_ERROR();
}

void Shader::loadProgram( int id, const char** sources, int* lengths )
{
  const String& name = library.shaders[id].name;

  log.println( "Creating program '%s' {", name.cstr() );
  log.indent();

  programs[id].vertShader = glCreateShader( GL_VERTEX_SHADER );
  programs[id].fragShader = glCreateShader( GL_FRAGMENT_SHADER );

  compileShader( programs[id].vertShader, "glsl/" + name + ".vert", sources, lengths );
  compileShader( programs[id].fragShader, "glsl/" + name + ".frag", sources, lengths );

  programs[id].program = glCreateProgram();
  glAttachShader( programs[id].program, programs[id].vertShader );
  glAttachShader( programs[id].program, programs[id].fragShader );

  log.print( "Linking ..." );

  OZ_REGISTER_ATTRIBUTE( Attrib::POSITION,            "inPosition" );
  OZ_REGISTER_ATTRIBUTE( Attrib::TEXCOORD,            "inTexCoord" );
  OZ_REGISTER_ATTRIBUTE( Attrib::NORMAL,              "inNormal" );
#ifdef OZ_BUMPMAP
  OZ_REGISTER_ATTRIBUTE( Attrib::TANGENT,             "inTangent" );
  OZ_REGISTER_ATTRIBUTE( Attrib::BINORMAL,            "inBinormal" );
#endif

  glLinkProgram( programs[id].program );

  int result;
  glGetProgramiv( programs[id].program, GL_LINK_STATUS, &result );
  if( result != GL_TRUE ) {
    char* logBuffer = new char[BUFFER_SIZE];
    int length;

    glGetProgramInfoLog( programs[id].program, BUFFER_SIZE, &length, logBuffer );
    logBuffer[BUFFER_SIZE - 1] = '\0';

    log.printEnd( " Error:" );
    log.printRaw( "\n%s", logBuffer );
    delete[] logBuffer;

    throw Exception( "Shader program linking failed" );
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

  OZ_REGISTER_PARAMETER( oz_Fog_start,                "oz_Fog.start" );
  OZ_REGISTER_PARAMETER( oz_Fog_end,                  "oz_Fog.end" );
  OZ_REGISTER_PARAMETER( oz_Fog_colour,               "oz_Fog.colour" );

  OZ_REGISTER_PARAMETER( oz_WaveBias,                 "oz_WaveBias" );
  OZ_REGISTER_PARAMETER( oz_Wind,                     "oz_Wind" );
  OZ_REGISTER_PARAMETER( oz_MD2Anim,                  "oz_MD2Anim" );

  param = programs[id].param;

  if( config.get( "shader.setSamplerIndices", true ) ) {
    int textureIds[4] = { 0, 1, 2, 3 };
    glUniform1iv( param.oz_Textures, 4, textureIds );
  }

  OZ_GL_CHECK_ERROR();

  log.printEnd( " OK" );

  log.unindent();
  log.println( "}" );
}

Shader::Shader() : plain( -1 )
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

void Shader::load()
{
  log.println( "Loading Shader {" );
  log.indent();

  const char* sources[3];
  int         lengths[3];

  sources[0] = defines;
  lengths[0] = defines.length();

  log.print( "Reading 'glsl/header.glsl' ..." );

  Buffer buffer = File( "glsl/header.glsl" ).read();
  if( buffer.isEmpty() ) {
    log.printEnd( " Failed" );
    throw Exception( "Shader loading failed" );
  }

  sources[1] = buffer.begin();
  lengths[1] = buffer.length();

  log.printEnd( " OK" );

  for( int i = 0; i < library.shaders.length(); ++i ) {
    if( i == plain ) {
      continue;
    }

    loadProgram( i, sources, lengths );
  }

  isLoaded = true;

  log.unindent();
  log.println( "}" );
}

void Shader::unload()
{
  log.print( "Unloading Shader ..." );

  isLoaded = false;

  for( int i = 0; i < library.shaders.length(); ++i ) {
    if( i == plain ) {
      continue;
    }

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

  OZ_GL_CHECK_ERROR();
  hard_assert( tf.stack.isEmpty() );

  log.printEnd( " OK" );
}

void Shader::init()
{
  log.println( "Initialising Shader {" );
  log.indent();

  bool isDeferred = config.get( "render.deferred", false );
  hasVertexTexture = config.get( "shader.vertexTexture", true );

  // bind white texture to id 0 to emulate fixed functionality (in fixed functionality sampler
  // always returns white colour when texture 0 is bound)
  ubyte whitePixel[] = { 0xff, 0xff, 0xff };

  for( int i = 0; i < 2; ++i ) {
    glActiveTexture( GL_TEXTURE0 + uint( i ) );
    glEnable( GL_TEXTURE_2D );

    glBindTexture( GL_TEXTURE_2D, 0 );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, whitePixel );
  }

  glActiveTexture( GL_TEXTURE0 );

  programs.alloc( library.shaders.length() );
  for( int i = 0; i < library.shaders.length(); ++i ) {
    programs[i].program    = 0;
    programs[i].vertShader = 0;
    programs[i].fragShader = 0;
  }

  const char* sources[3];
  int         lengths[3];

  defines = "#version 120\n";
  defines = defines + ( isDeferred ?       "#define OZ_DEFERRED\n" : "\n" );
  defines = defines + ( hasVertexTexture ? "#define OZ_VERTEX_TEXTURE\n" : "\n" );

  for( int i = 3; i < 10; ++i ) {
    defines = defines + "\n";
  }

  sources[0] = defines;
  lengths[0] = defines.length();

  log.print( "Reading 'glsl/header.glsl' ..." );

  Buffer buffer = File( "glsl/header.glsl" ).read();
  if( buffer.isEmpty() ) {
    log.printEnd( " Failed" );
    throw Exception( "Shader loading failed" );
  }

  sources[1] = buffer.begin();
  lengths[1] = buffer.length();

  log.printEnd( " OK" );

  plain     = library.shaderIndex( "plain" );
  mesh      = library.shaderIndex( "mesh" );
  combine   = library.shaderIndex( "combine" );
  colour    = Vec4::ONE;
  isInWater = false;
  isLoaded  = false;

  loadProgram( plain, sources, lengths );

  log.unindent();
  log.println( "}" );
}

void Shader::free()
{
  log.print( "Shutting down Shader ..." );

  if( plain != -1 && programs[plain].program != 0 ) {
    glDetachShader( programs[plain].program, programs[plain].vertShader );
    glDetachShader( programs[plain].program, programs[plain].fragShader );
    glDeleteProgram( programs[plain].program );
    programs[plain].program = 0;
  }
  if( plain != -1 && programs[plain].vertShader != 0 ) {
    glDeleteShader( programs[plain].vertShader );
    programs[plain].vertShader = 0;
  }
  if( plain != -1 && programs[plain].fragShader != 0 ) {
    glDeleteShader( programs[plain].fragShader );
    programs[plain].fragShader = 0;
  }

  plain = -1;
  defines = "";

  programs.dealloc();

  OZ_GL_CHECK_ERROR();

  log.printEnd( " OK" );
}

}
}
