/*
 *  Shader.cpp
 *
 *  Shader utilities
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "client/common.hpp"
#include "client/Shader.hpp"

#include <GL/gl.h>

#define OZ_REGISTER_PARAMETER( param ) \
  Param::param = glGetUniformLocation( programs[prog], #param );

namespace oz
{
namespace client
{

  Shader shader;

  int Param::oz_TextureEnabled;
  int Param::oz_Textures;
  int Param::oz_TextureScale;

  int Param::oz_DiffuseMaterial;
  int Param::oz_SpecularMaterial;

  int Param::oz_AmbientLight;
  int Param::oz_SkyLight;
  int Param::oz_PointLights;

  const char* Shader::PROGRAM_NAMES[MAX] = {
    "ui",
    "default",
    "stars"
  };

  const Shader::Light Shader::Light::NONE = Shader::Light( Point3::ORIGIN, Quat::ZERO );

  void Shader::compileShader( uint id, const char* path, const char** sources, int* lengths ) const
  {
    log.print( "Compiling '%s' ...", path );

    Buffer buffer( path );
    if( buffer.isEmpty() ) {
      log.printEnd( " No such file" );
      throw Exception( "Shader source read failed" );
    }

    if( buffer.length() != 0 && *( buffer.end() - 1 ) != '\n' ) {
      log.printEnd( " Last line not terminated" );
      throw Exception( "Shader source has invalid format" );
    }

    sources[2] = buffer.begin();
    lengths[2] = buffer.length();

    glShaderSource( id, 3, sources, lengths );

    buffer.dealloc();

    glCompileShader( id );

    int result;
    glGetShaderiv( id, GL_COMPILE_STATUS, &result );
    hard_assert( glGetError() == GL_NO_ERROR );

    if( result != GL_TRUE ) {
      char* logBuffer = new char[BUFFER_SIZE];
      int length;

      glGetShaderInfoLog( id, BUFFER_SIZE, &length, logBuffer );
      logBuffer[BUFFER_SIZE - 1] = '\0';

      log.printEnd( " Error:" );
      log.printRaw( "\n%s", logBuffer );
      delete[] logBuffer;

      throw Exception( "Shader compile failed" );
    }

    hard_assert( glGetError() == GL_NO_ERROR );

    log.printEnd( " OK" );
  }

  void Shader::loadProgram( Program prog, const char** sources, int* lengths )
  {
    log.println( "Creating program '%s' {", PROGRAM_NAMES[prog] );
    log.indent();

    vertShaders[prog] = glCreateShader( GL_VERTEX_SHADER );
    fragShaders[prog] = glCreateShader( GL_FRAGMENT_SHADER );

    compileShader( vertShaders[prog], "glsl/" + String( PROGRAM_NAMES[prog] ) + ".vert",
                   sources, lengths );
    compileShader( fragShaders[prog], "glsl/" + String( PROGRAM_NAMES[prog] ) + ".frag",
                   sources, lengths );

    programs[prog] = glCreateProgram();
    glAttachShader( programs[prog], vertShaders[prog] );
    glAttachShader( programs[prog], fragShaders[prog] );

    log.print( "Linking ..." );

    glLinkProgram( programs[prog] );

    int result;
    glGetProgramiv( programs[prog], GL_LINK_STATUS, &result );
    if( result != GL_TRUE ) {
      char* logBuffer = new char[BUFFER_SIZE];
      int length;

      glGetProgramInfoLog( programs[prog], BUFFER_SIZE, &length, logBuffer );
      logBuffer[BUFFER_SIZE - 1] = '\0';

      log.printEnd( " Error:" );
      log.printRaw( "\n%s", logBuffer );
      delete[] logBuffer;

      throw Exception( "Shader program linking failed" );
    }

    hard_assert( glGetError() == GL_NO_ERROR );

    log.printEnd( " OK" );

    log.unindent();
    log.println( "}" );
  }

  void Shader::bindTextures( uint texture0, uint texture1 ) const
  {
    glActiveTexture( GL_TEXTURE1 );
    glBindTexture( GL_TEXTURE_2D, texture1 );

    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, texture0 );

    if( texture0 == 0 ) {
      glUniform1i( Param::oz_TextureEnabled, 0 );
    }
    else {
      glUniform1i( Param::oz_TextureEnabled, 1 );
    }

    hard_assert( glGetError() == GL_NO_ERROR );
  }

  void Shader::use( Program prog )
  {
    activeProgram = prog;

    glUseProgram( programs[prog] );

    OZ_REGISTER_PARAMETER( oz_TextureEnabled );
    OZ_REGISTER_PARAMETER( oz_Textures );
    OZ_REGISTER_PARAMETER( oz_TextureScale );

    OZ_REGISTER_PARAMETER( oz_DiffuseMaterial );
    OZ_REGISTER_PARAMETER( oz_SpecularMaterial );

    OZ_REGISTER_PARAMETER( oz_AmbientLight );
    OZ_REGISTER_PARAMETER( oz_SkyLight );
    OZ_REGISTER_PARAMETER( oz_PointLights );

    hard_assert( glGetError() == GL_NO_ERROR );
  }

  void Shader::load()
  {
    log.println( "Loading Shader {" );
    log.indent();

    const char* sources[3];
    int         lengths[3];

    sources[0] = "";
    lengths[0] = 0;

    log.print( "Reading 'glsl/header.glsl' ..." );

    Buffer buffer;
    if( !buffer.read( "glsl/header.glsl" ) ) {
      log.printEnd( " Failed" );
      throw Exception( "Shader loading failed" );
    }

    sources[1] = buffer.begin();
    lengths[1] = buffer.length();

    log.printEnd( " OK" );

    for( int i = 1; i < MAX; ++i ) {
      loadProgram( Program( i ), sources, lengths );
    }

    log.unindent();
    log.println( "}" );
  }

  void Shader::unload()
  {
    log.print( "Unloading Shader ..." );

    for( int i = 1; i < MAX; ++i ) {
      if( programs[i] != 0 ) {
        glDetachShader( programs[i], vertShaders[i] );
        glDetachShader( programs[i], fragShaders[i] );
        glDeleteProgram( programs[i] );
        programs[i] = 0;
      }
      if( vertShaders[i] != 0 ) {
        glDeleteShader( vertShaders[i] );
        vertShaders[i] = 0;
      }
      if( fragShaders[i] != 0 ) {
        glDeleteShader( fragShaders[i] );
        fragShaders[i] = 0;
      }
    }

    hard_assert( glGetError() == GL_NO_ERROR );

    log.printEnd( " OK" );
  }

  void Shader::init()
  {
    aSet<uint>( vertShaders, 0, MAX );
    aSet<uint>( fragShaders, 0, MAX );
    aSet<uint>( programs,  0, MAX );

    log.println( "Initialising Shader {" );
    log.indent();

    const char* sources[3];
    int         lengths[3];

    sources[0] = "";
    lengths[0] = 0;

    log.print( "Reading 'glsl/header.glsl' ..." );

    Buffer buffer;
    if( !buffer.read( "glsl/header.glsl" ) ) {
      log.printEnd( " Failed" );
      throw Exception( "Shader loading failed" );
    }

    sources[1] = buffer.begin();
    lengths[1] = buffer.length();

    log.printEnd( " OK" );

    loadProgram( Program( 0 ), sources, lengths );

    log.unindent();
    log.println( "}" );
  }

  void Shader::free()
  {
    log.print( "Shutting down Shader ..." );

    if( programs[0] != 0 ) {
      glDetachShader( programs[0], vertShaders[0] );
      glDetachShader( programs[0], fragShaders[0] );
      glDeleteProgram( programs[0] );
      programs[0] = 0;
    }
    if( vertShaders[0] != 0 ) {
      glDeleteShader( vertShaders[0] );
      vertShaders[0] = 0;
    }
    if( fragShaders[0] != 0 ) {
      glDeleteShader( fragShaders[0] );
      fragShaders[0] = 0;
    }

    hard_assert( glGetError() == GL_NO_ERROR );

    log.printEnd( " OK" );
  }

}
}
