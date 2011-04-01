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

    glUniformMatrix4fv( param.oz_Transform_proj, 1, GL_FALSE, proj );
    glUniformMatrix4fv( param.oz_Transform_camera, 1, GL_FALSE, camera );
    glUniformMatrix4fv( param.oz_Transform_projCamera, 1, GL_FALSE, projCamera );

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

  void Shader::loadProgram( int id, const char** sources, int* lengths )
  {
    const String& name = translator.shaders[id].name;

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
//     OZ_REGISTER_ATTRIBUTE( Attrib::TANGENT,             "inTangent" );
//     OZ_REGISTER_ATTRIBUTE( Attrib::BINORMAL,            "inBinormal" );

    OZ_REGISTER_FRAGDATA( FragData::COLOUR,             "outColour" );
//     OZ_REGISTER_FRAGDATA( FragData::EFFECT,             "outEffect" );
//     OZ_REGISTER_FRAGDATA( FragData::NORMAL,             "outNormal" );

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

    OZ_REGISTER_PARAMETER( oz_Transform_proj,           "oz_Transform.proj" );
    OZ_REGISTER_PARAMETER( oz_Transform_camera,         "oz_Transform.camera" );
    OZ_REGISTER_PARAMETER( oz_Transform_projCamera,     "oz_Transform.projCamera" );
    OZ_REGISTER_PARAMETER( oz_Transform_model,          "oz_Transform.model" );
    OZ_REGISTER_PARAMETER( oz_Transform_complete,       "oz_Transform.complete" );

    OZ_REGISTER_PARAMETER( oz_CameraPosition,           "oz_CameraPosition" );

    OZ_REGISTER_PARAMETER( oz_Colour,                   "oz_Colour" );

    OZ_REGISTER_PARAMETER( oz_IsTextureEnabled,         "oz_IsTextureEnabled" );
    OZ_REGISTER_PARAMETER( oz_Textures,                 "oz_Textures" );
    OZ_REGISTER_PARAMETER( oz_TextureScales,            "oz_TextureScales" );

    OZ_REGISTER_PARAMETER( oz_CaelumLight_dir,          "oz_CaelumLight.dir" );
    OZ_REGISTER_PARAMETER( oz_CaelumLight_diffuse,      "oz_CaelumLight.diffuse" );
    OZ_REGISTER_PARAMETER( oz_CaelumLight_ambient,      "oz_CaelumLight.ambient" );
    OZ_REGISTER_PARAMETER( oz_PointLights,              "oz_PointLights" );

    OZ_REGISTER_PARAMETER( oz_Specular,                 "oz_Specular" );

    OZ_REGISTER_PARAMETER( oz_Fog_start,                "oz_Fog.start" );
    OZ_REGISTER_PARAMETER( oz_Fog_end,                  "oz_Fog.end" );
    OZ_REGISTER_PARAMETER( oz_Fog_colour,               "oz_Fog.colour" );

    OZ_REGISTER_PARAMETER( oz_WaveBias,                 "oz_WaveBias" );

    OZ_REGISTER_PARAMETER( oz_Wind,                     "oz_Wind" );

    OZ_REGISTER_PARAMETER( oz_MD2Anim,                  "oz_MD2Anim" );

    param = programs[id].param;

    glUniform1iv( param.oz_Textures, 4, (int[]) { 0, 1, 2, 3 } );

    hard_assert( glGetError() == GL_NO_ERROR );

    log.printEnd( " OK" );

    log.unindent();
    log.println( "}" );
  }

  Shader::Shader() : ui( -1 )
  {}

  void Shader::use( int id )
  {
    if( id == activeProgram ) {
      return;
    }

    activeProgram = id;

    glUseProgram( programs[id].program );
    param = programs[id].param;

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
    caelumLight.ambient = colour;
  }

  void Shader::setCaelumLight( const Vec3& dir, const Vec4& colour )
  {
    caelumLight.dir = dir;
    caelumLight.diffuse = colour;
  }

//   int Shader::addLight( const Point3& pos, const Vec4& colour )
//   {
//     return lights.add( Light( pos, colour ) );
//   }
//
//   void Shader::removeLight( int id )
//   {
//     lights.remove( id );
//   }
//
//   void Shader::setLight( int id, const Point3& pos, const Vec4& colour )
//   {
//     lights[id].pos = pos;
//     lights[id].diffuse = colour;
//   }

  void Shader::updateLights()
  {
    glUniform3fv( param.oz_CaelumLight_dir,     1, caelumLight.dir );
    glUniform4fv( param.oz_CaelumLight_diffuse, 1, caelumLight.diffuse );
    glUniform4fv( param.oz_CaelumLight_ambient, 1, caelumLight.ambient );

//     Map<float, const Light*> localLights( lights.length() );

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

    defines = "#version 130\n";
    defines = defines + ( hasVertexTexture ? "#define OZ_VERTEX_TEXTURE\n" : "\n" );

    for( int i = 2; i < 10; ++i ) {
      defines = defines + "\n";
    }

    sources[0] = defines;
    lengths[0] = defines.length();

    log.print( "Reading 'glsl/header.glsl' ..." );

    Buffer buffer;
    if( !buffer.read( "glsl/header.glsl" ) ) {
      log.printEnd( " Failed" );
      throw Exception( "Shader loading failed" );
    }

    sources[1] = buffer.begin();
    lengths[1] = buffer.length();

    log.printEnd( " OK" );

    for( int i = 0; i < translator.shaders.length(); ++i ) {
      if( i == ui ) {
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

    for( int i = 0; i < translator.shaders.length(); ++i ) {
      if( i == ui ) {
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

    int error = glGetError();
    hard_assert( error == GL_NO_ERROR );

    hard_assert( tf.stack.isEmpty() );
    tf.stack.dealloc();

    log.printEnd( " OK" );
  }

  void Shader::init()
  {
    log.println( "Initialising Shader {" );
    log.indent();

    hasVertexTexture = config.getSet( "shader.vertexTexture", false );

    programs.alloc( translator.shaders.length() );
    for( int i = 0; i < translator.shaders.length(); ++i ) {
      programs[i].program    = 0;
      programs[i].vertShader = 0;
      programs[i].fragShader = 0;
    }

    const char* sources[3];
    int         lengths[3];

    defines = "#version 130\n";
    defines = defines + ( hasVertexTexture ? "#define OZ_VERTEX_TEXTURE\n" : "\n" );

    for( int i = 2; i < 10; ++i ) {
      defines = defines + "\n";
    }

    sources[0] = defines;
    lengths[0] = defines.length();

    log.print( "Reading 'glsl/header.glsl' ..." );

    Buffer buffer;
    if( !buffer.read( "glsl/header.glsl" ) ) {
      log.printEnd( " Failed" );
      throw Exception( "Shader loading failed" );
    }

    sources[1] = buffer.begin();
    lengths[1] = buffer.length();

    log.printEnd( " OK" );

    ui        = translator.shaderIndex( "ui" );
    mesh      = translator.shaderIndex( "mesh" );
    bigMesh   = translator.shaderIndex( "bigMesh" );
    colour    = Vec4::ONE;
    isInWater = false;
    isLoaded  = false;

    loadProgram( ui, sources, lengths );

    log.unindent();
    log.println( "}" );
  }

  void Shader::free()
  {
    log.print( "Shutting down Shader ..." );

    if( ui != -1 && programs[ui].program != 0 ) {
      glDetachShader( programs[ui].program, programs[ui].vertShader );
      glDetachShader( programs[ui].program, programs[ui].fragShader );
      glDeleteProgram( programs[ui].program );
      programs[ui].program = 0;
    }
    if( ui != -1 && programs[ui].vertShader != 0 ) {
      glDeleteShader( programs[ui].vertShader );
      programs[ui].vertShader = 0;
    }
    if( ui != -1 && programs[ui].fragShader != 0 ) {
      glDeleteShader( programs[ui].fragShader );
      programs[ui].fragShader = 0;
    }

    defines = "";

    programs.dealloc();

    hard_assert( glGetError() == GL_NO_ERROR );

    log.printEnd( " OK" );
  }

}
}
