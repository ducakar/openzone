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
  progParams[prog].param = glGetUniformLocation( programs[prog], #param )

namespace oz
{
namespace client
{

  Param  param;
  Shader shader;

  const char* Shader::PROGRAM_NAMES[MAX] = {
    "ui",
    "mesh_itemview",
    "mesh_near",
    "mesh_far",
    "mesh_water",
    "terra",
    "terra_water",
    "stars"
  };

  const Shader::Light Shader::Light::NONE = Shader::Light( Point3::ORIGIN, Quat::ZERO );

  Shader::Light::Light( const Point3& pos_, const Vec3& diffuse_ )
  {
    for( int i = 0; i < 3; ++i ) {
      pos[i] = pos_[i];
      diffuse[i] = diffuse_[i];
    }
  }

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

    glUseProgram( programs[prog] );

    OZ_REGISTER_PARAMETER( oz_IsTextureEnabled );
    OZ_REGISTER_PARAMETER( oz_Textures );
    OZ_REGISTER_PARAMETER( oz_TextureScales );

    OZ_REGISTER_PARAMETER( oz_DiffuseMaterial );
    OZ_REGISTER_PARAMETER( oz_SpecularMaterial );
    OZ_REGISTER_PARAMETER( oz_AmbientLight );
    OZ_REGISTER_PARAMETER( oz_SkyLight );
    OZ_REGISTER_PARAMETER( oz_PointLights );

    OZ_REGISTER_PARAMETER( oz_FogDistance );
    OZ_REGISTER_PARAMETER( oz_FogColour );

    OZ_REGISTER_PARAMETER( oz_IsHighlightEnabled );

    param = progParams[prog];

    glUniform1i( param.oz_IsTextureEnabled, false );
    glUniform1iv( param.oz_Textures, 2, (int[2]) { 0, 1 } );
    glUniform1fv( param.oz_TextureScales, 2, (float[2]) { 1.0f, 1.0f } );

    glUniform1i( param.oz_IsHighlightEnabled, false );

    hard_assert( glGetError() == GL_NO_ERROR );

    log.printEnd( " OK" );

    log.unindent();
    log.println( "}" );
  }

  void Shader::use( Program prog )
  {
    activeProgram = prog;

    glUseProgram( programs[prog] );
    param = progParams[prog];

    hard_assert( glGetError() == GL_NO_ERROR );
  }

  void Shader::setLightingDistance( float distance )
  {
    lightingDistance = distance;
  }

  void Shader::setAmbientLight( const Vec3& colour )
  {
    for( int i = 0; i < 3; ++i ) {
      skyLight.ambient[i] = colour[i];
    }
  }

  void Shader::setSkyLight( const Vec3& dir, const Vec3& colour )
  {
    for( int i = 0; i < 3; ++i ) {
      skyLight.dir[i] = dir[i];
      skyLight.diffuse[i] = colour[i];
    }
  }

  int Shader::addLight( const Point3& pos, const Vec3& colour )
  {
    return lights.add( Light( pos, colour ) );
  }

  void Shader::removeLight( int id )
  {
    lights.remove( id );
  }

  void Shader::setLight( int id, const Point3& pos, const Vec3& colour )
  {
    for( int i = 0; i < 3; ++i ) {
      lights[id].pos[i] = pos[i];
      lights[id].diffuse[i] = colour[i];
    }
  }

  void Shader::updateLights()
  {
    glUniform3fv( param.oz_SkyLight, 3, skyLight.dir );

    Map<float, const Light*> localLights( lights.length() );

//     Mat44 transf;
//     glGetFloatv( GL_MODELVIEW_MATRIX, transf );

//     foreach( light, lights.citer() ) {
//       Point3 localPos = transf * Point3( light->pos );
//       float  dist     = ( localPos - Point3::ORIGIN ).sqL();
//
//       if( dist < lightingDistance ) {
//         localLights.add( dist, light );
//       }
//     }

    // TODO
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

    textures[0] = 0;
    textures[1] = 0;

    log.unindent();
    log.println( "}" );
  }

  void Shader::unload()
  {
    log.print( "Unloading Shader ..." );

    textures[0] = 0;
    textures[1] = 0;

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
