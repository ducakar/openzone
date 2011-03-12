/*
 *  Shader.cpp
 *
 *  Shader utilities
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "client/Shader.hpp"

#include "client/Camera.hpp"

#include <GL/gl.h>

#define OZ_REGISTER_PARAMETER( paramName, paramUniform ) \
  progParams[prog].paramName = glGetUniformLocation( programs[prog], paramUniform )

namespace oz
{
namespace client
{

  Param     param;
  Transform tf;
  Shader    shader;

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

    glUniformMatrix4fv( param.oz_Transform_proj, 1, GL_FALSE, proj );
    glUniformMatrix4fv( param.oz_Transform_camera, 1, GL_FALSE, camera );
    glUniformMatrix4fv( param.oz_Transform_projCamera, 1, GL_FALSE, projCamera );
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

  const char* Shader::PROGRAM_NAMES[MAX] = {
    "ui",
    "text",
    "simple",
    "mesh_itemview",
    "mesh_near",
    "mesh_far",
    "mesh_water",
    "terra",
    "terra_water",
    "stars",
    "particles",
    "md2"
  };

  const Shader::Light Shader::Light::NONE = Shader::Light( Point3::ORIGIN, Vec4::ZERO );

  Shader::Light::Light( const Point3& pos_, const Vec4& diffuse_ ) :
      pos( pos_ ), diffuse( diffuse_ )
  {}

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

    char* logBuffer = new char[BUFFER_SIZE];
    int length;

    glGetShaderInfoLog( id, BUFFER_SIZE, &length, logBuffer );
    logBuffer[BUFFER_SIZE - 1] = '\0';

    if( result != GL_TRUE ) {
      log.printEnd( " Failed:" );
      log.printRaw( "%s", logBuffer );
    }
    else if( length != 0 ) {
      log.printEnd( " OK, but:" );
      log.printRaw( "%s\n", logBuffer );
    }
    else {
      log.printEnd( " OK" );
    }

    delete[] logBuffer;

    if( result != GL_TRUE ) {
      throw Exception( "Shader compile failed" );
    }

    hard_assert( glGetError() == GL_NO_ERROR );
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

    OZ_REGISTER_PARAMETER( oz_Transform_proj,           "oz_Transform.proj" );
    OZ_REGISTER_PARAMETER( oz_Transform_camera,         "oz_Transform.camera" );
    OZ_REGISTER_PARAMETER( oz_Transform_projCamera,     "oz_Transform.projCamera" );
    OZ_REGISTER_PARAMETER( oz_Transform_model,          "oz_Transform.model" );
    OZ_REGISTER_PARAMETER( oz_Transform_complete,       "oz_Transform.complete" );

    OZ_REGISTER_PARAMETER( oz_Colour,                   "oz_Colour" );

    OZ_REGISTER_PARAMETER( oz_IsTextureEnabled,         "oz_IsTextureEnabled" );
    OZ_REGISTER_PARAMETER( oz_Textures,                 "oz_Textures" );
    OZ_REGISTER_PARAMETER( oz_TextureScales,            "oz_TextureScales" );

    OZ_REGISTER_PARAMETER( oz_SpecularMaterial,         "oz_SpecularMaterial" );

    OZ_REGISTER_PARAMETER( oz_SkyLight_dir,             "oz_SkyLight.dir" );
    OZ_REGISTER_PARAMETER( oz_SkyLight_diffuse,         "oz_SkyLight.diffuse" );
    OZ_REGISTER_PARAMETER( oz_SkyLight_ambient,         "oz_SkyLight.ambient" );
    OZ_REGISTER_PARAMETER( oz_PointLights,              "oz_PointLights" );

    OZ_REGISTER_PARAMETER( oz_NearDistance,             "oz_NearDistance" );

    OZ_REGISTER_PARAMETER( oz_FogDistance,              "oz_FogDistance" );
    OZ_REGISTER_PARAMETER( oz_FogColour,                "oz_FogColour" );

    OZ_REGISTER_PARAMETER( oz_MD2Anim,                  "oz_MD2Anim" );

    param = progParams[prog];

    glUniform1iv( param.oz_Textures, 2, (int[2]) { 0, 1 } );

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
    skyLight.ambient = colour;
  }

  void Shader::setSkyLight( const Vec3& dir, const Vec4& colour )
  {
    skyLight.dir = dir;
    skyLight.diffuse = colour;
  }

  int Shader::addLight( const Point3& pos, const Vec4& colour )
  {
    return lights.add( Light( pos, colour ) );
  }

  void Shader::removeLight( int id )
  {
    lights.remove( id );
  }

  void Shader::setLight( int id, const Point3& pos, const Vec4& colour )
  {
    lights[id].pos = pos;
    lights[id].diffuse = colour;
  }

  void Shader::updateLights()
  {
    glUniform3fv( param.oz_SkyLight_dir,     1, skyLight.dir );
    glUniform4fv( param.oz_SkyLight_diffuse, 1, skyLight.diffuse );
    glUniform4fv( param.oz_SkyLight_ambient, 1, skyLight.ambient );

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

    for( int i = 2; i < MAX; ++i ) {
      loadProgram( Program( i ), sources, lengths );
    }

    colour = Vec4::ONE;

    log.unindent();
    log.println( "}" );
  }

  void Shader::unload()
  {
    log.print( "Unloading Shader ..." );

    for( int i = 2; i < MAX; ++i ) {
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
    log.println( "Initialising Shader {" );
    log.indent();

    aSet<uint>( vertShaders, 0, MAX );
    aSet<uint>( fragShaders, 0, MAX );
    aSet<uint>( programs,  0, MAX );

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
    loadProgram( Program( 1 ), sources, lengths );

    log.unindent();
    log.println( "}" );
  }

  void Shader::free()
  {
    log.print( "Shutting down Shader ..." );

    for( int i = 0; i < 2; ++i ) {
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

}
}
