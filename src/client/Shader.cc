/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2013 Davorin Učakar
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

#include <client/Shader.hh>

#include <client/Camera.hh>

#define OZ_REGISTER_UNIFORM( uniformVar, uniformName ) \
  programs[id].uniform.uniformVar = glGetUniformLocation( programs[id].program, uniformName )

#define OZ_REGISTER_ATTRIBUTE( location, name ) \
  glBindAttribLocation( programs[id].program, location, name )

namespace oz
{
namespace client
{

Uniform uniform;

void Transform::ortho( int width, int height )
{
  float cx = float( 2.0 / double( width ) );
  float cy = float( 2.0 / double( height ) );

  proj = Mat44(    cx,  0.0f,    0.0f, 0.0f,
                 0.0f,    cy,    0.0f, 0.0f,
                 0.0f,  0.0f, -0.001f, 0.0f,
                -1.0f, -1.0f,    0.0f, 1.0f );
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
  glUniformMatrix4fv( uniform.projCamera, 1, GL_FALSE, proj * camera );
  glUniform3fv( uniform.cameraPos, 1, client::camera.p );
}

void Transform::apply() const
{
  glUniformMatrix4fv( uniform.model, 1, GL_FALSE, model );
  glUniformMatrix3fv( uniform.modelRot, 1, GL_FALSE, model.mat33() );
}

void Transform::applyColour() const
{
  setColour( colour );
}

void Transform::setColour( const Mat44& colour_ ) const
{
  glUniformMatrix4fv( uniform.colour, 1, GL_FALSE, colour_ );
}

void Transform::setColour( const Vec4& colour_ ) const
{
  setColour( Mat44::scaling( colour_ ) );
}

void Transform::setColour( float r, float g, float b, float a ) const
{
  setColour( Mat44::scaling( Vec4( r, g, b, a ) ) );
}

Transform tf;

const int Shader::SAMPLER_MAP[] = { 0, 1, 2 };
char      Shader::logBuffer[LOG_BUFFER_SIZE];
String    Shader::defines;

Shader::Light::Light( const Point& pos_, const Vec4& diffuse_ ) :
  pos( pos_ ), diffuse( diffuse_ )
{}

void Shader::compileShader( uint shaderId, const char* defines, const char* path ) const
{
  bool hasCompiled = GL::compileShaderFromFile( shaderId, defines, path );

  int length;
  glGetShaderInfoLog( shaderId, LOG_BUFFER_SIZE, &length, logBuffer );
  logBuffer[LOG_BUFFER_SIZE - 1] = '\0';

  if( length != 0 ) {
    if( hasCompiled ) {
      Log::verboseMode = true;
    }

    Log::printRaw( "\n%s:\n%s", path, logBuffer );
    Log::verboseMode = false;
  }

  if( !hasCompiled ) {
    OZ_ERROR( "Shader '%s' compile failed", path );
  }

  OZ_GL_CHECK_ERROR();
}

void Shader::loadProgram( int id )
{
  const String& name = liber.shaders[id].name;

  File configFile = "@glsl/" + name + ".json";
  JSON programConfig;

  if( !programConfig.load( configFile ) ) {
    OZ_ERROR( "Failed to read shader program configuration '%s'", configFile.path().cstr() );
  }

  const char* vertName = programConfig[0].get( "" );
  const char* fragName = programConfig[1].get( "" );

  const uint* vertId = vertShaders.find( vertName );
  const uint* fragId = fragShaders.find( fragName );

  if( vertId == nullptr ) {
    OZ_ERROR( "Invalid vertex shader '%s' requested for shader program '%s'",
              vertName, name.cstr() );
  }
  if( fragId == nullptr ) {
    OZ_ERROR( "Invalid fragment shader '%s' requested for shader program '%s'",
              fragName, name.cstr() );
  }

  MainCall() << [&]()
  {
    programs[id].vertShader = *vertId;
    programs[id].fragShader = *fragId;
    programs[id].program    = glCreateProgram();

    glAttachShader( programs[id].program, programs[id].vertShader );
    glAttachShader( programs[id].program, programs[id].fragShader );

    OZ_REGISTER_ATTRIBUTE( Attrib::POSITION, "inPosition" );
    OZ_REGISTER_ATTRIBUTE( Attrib::TEXCOORD, "inTexCoord" );
    OZ_REGISTER_ATTRIBUTE( Attrib::NORMAL,   "inNormal"   );
    OZ_REGISTER_ATTRIBUTE( Attrib::TANGENT,  "inTangent"  );
    OZ_REGISTER_ATTRIBUTE( Attrib::BINORMAL, "inBinormal" );

    glLinkProgram( programs[id].program );

    int result;
    glGetProgramiv( programs[id].program, GL_LINK_STATUS, &result );

    int length;
    glGetProgramInfoLog( programs[id].program, LOG_BUFFER_SIZE, &length, logBuffer );
    logBuffer[LOG_BUFFER_SIZE - 1] = '\0';

    if( length != 0 ) {
      if( result == GL_TRUE ) {
        Log::verboseMode = true;
      }

      Log::printRaw( "\n%s:\n%s", name.cstr(), logBuffer );
      Log::verboseMode = false;
    }

    if( result != GL_TRUE ) {
      OZ_ERROR( "Shader program '%s' linking failed", name.cstr() );
    }

    glUseProgram( programs[id].program );

    OZ_REGISTER_UNIFORM( projCamera,          "oz_ProjCamera"          );
    OZ_REGISTER_UNIFORM( model,               "oz_Model"               );
    OZ_REGISTER_UNIFORM( modelRot,            "oz_ModelRot"            );
    OZ_REGISTER_UNIFORM( cameraPos,           "oz_CameraPos"           );
    OZ_REGISTER_UNIFORM( bones,               "oz_Bones"               );
    OZ_REGISTER_UNIFORM( meshAnimation,       "oz_MeshAnimation"       );

    OZ_REGISTER_UNIFORM( colour,              "oz_Colour"              );
    OZ_REGISTER_UNIFORM( textures,            "oz_Textures"            );
    OZ_REGISTER_UNIFORM( envMap,              "oz_EnvMap"              );
    OZ_REGISTER_UNIFORM( vertexAnim,          "oz_VertexAnim"          );

    OZ_REGISTER_UNIFORM( caelumLight_dir,     "oz_CaelumLight.dir"     );
    OZ_REGISTER_UNIFORM( caelumLight_diffuse, "oz_CaelumLight.diffuse" );
    OZ_REGISTER_UNIFORM( caelumLight_ambient, "oz_CaelumLight.ambient" );

    OZ_REGISTER_UNIFORM( fog_colour,          "oz_Fog.colour"          );
    OZ_REGISTER_UNIFORM( fog_dist2,           "oz_Fog.dist2"           );

    OZ_REGISTER_UNIFORM( starsColour,         "oz_StarsColour"         );
    OZ_REGISTER_UNIFORM( waveBias,            "oz_WaveBias"            );
    OZ_REGISTER_UNIFORM( wind,                "oz_Wind"                );

    uniform = programs[id].uniform;

    if( setSamplerMap ) {
      glUniform1iv( uniform.textures, aLength( SAMPLER_MAP ), SAMPLER_MAP );
      glUniform1i( uniform.envMap, 3 );

      if( hasVertexTexture ) {
        glUniform1i( uniform.vertexAnim, 4 );
      }
    }

    Mat44 bones[] = {
      Mat44::ID, Mat44::ID, Mat44::ID, Mat44::ID,
      Mat44::ID, Mat44::ID, Mat44::ID, Mat44::ID,
      Mat44::ID, Mat44::ID, Mat44::ID, Mat44::ID,
      Mat44::ID, Mat44::ID, Mat44::ID, Mat44::ID
    };

    glUniformMatrix4fv( uniform.bones, 16, GL_FALSE, bones[0] );

    glActiveTexture( ENV_MAP );
    glBindTexture( GL_TEXTURE_CUBE_MAP, noiseTexture );
    glActiveTexture( DIFFUSE );
  };

  OZ_GL_CHECK_ERROR();
}

Shader::Shader() :
  plain( -1 ), defaultMasks( 0 )
{}

void Shader::program( int id )
{
  if( id == activeProgram ) {
    return;
  }

  activeProgram = id;

  glUseProgram( programs[id].program );
  uniform = programs[id].uniform;

  OZ_GL_CHECK_ERROR();
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
  glUniform3fv( uniform.caelumLight_dir,     1, caelumLight.dir );
  glUniform3fv( uniform.caelumLight_diffuse, 1, caelumLight.diffuse );
  glUniform3fv( uniform.caelumLight_ambient, 1, caelumLight.ambient );
}

void Shader::init()
{
  Log::print( "Initialising Shader ..." );

  hasVertexTexture = config.include( "shader.vertexTexture", true  ).get( false );
  setSamplerMap    = config.include( "shader.setSamplerMap", true  ).get( false );
  doEnvMap         = config.include( "shader.envMap",        true  ).get( false );
  doBumpMap        = config.include( "shader.bumpMap",       true  ).get( false );
  isLowDetail      = config.include( "shader.lowDetail",     false ).get( false );
  doPostprocess    = config.include( "shader.postprocess",   false ).get( false );
  nLights          = config.include( "shader.nLights",       4     ).get( 0 );

#ifdef GL_ES_VERSION_2_0
  hasVertexTexture = false;
  doPostprocess    = false;
#endif

  medium = 0;

  // Bind white texture to id 0 to emulate fixed functionality that has white texture on id 0.
  ubyte whitePixel[] = { 0xff, 0xff, 0xff };

  glGenTextures( 1, &defaultTexture );
  glBindTexture( GL_TEXTURE_2D, defaultTexture );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
  glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, whitePixel );

  // Default masks: specular 0.0, emission 0.0, environment 0.0.
  ubyte masksPixel[] = { 0x00, 0x00, 0x00 };

  glGenTextures( 1, &defaultMasks );
  glBindTexture( GL_TEXTURE_2D, defaultMasks );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
  glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, masksPixel );

  // Default normal for bumpmap: [0, 0, 1].
  ubyte normalsPixel[] = { 0x80, 0x80, 0xff };

  glGenTextures( 1, &defaultNormals );
  glBindTexture( GL_TEXTURE_2D, defaultNormals );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
  glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, normalsPixel );

  File envMap = "@glsl/env.dds";

  glGenTextures( 1, &noiseTexture );
  glBindTexture( GL_TEXTURE_CUBE_MAP, noiseTexture );

  if( GL::textureDataFromFile( envMap, 0 ) == 0 ) {
    OZ_ERROR( "Failed to load environment map texture '%s'", envMap.path().cstr() );
  }

  glActiveTexture( DIFFUSE );
  glBindTexture( GL_TEXTURE_2D, defaultTexture );

  if( liber.shaders.length() == 0 ) {
    OZ_ERROR( "Shaders missing" );
  }

  plain       = liber.shaderIndex( "plain" );
  mesh        = liber.shaderIndex( "mesh" );
  postprocess = liber.shaderIndex( "postprocess" );

  programs.resize( liber.shaders.length() );

#ifdef GL_ES_VERSION_2_0
  defines = "#version 100\n";
#else
  defines = "#version 120\n";
#endif
  if( hasVertexTexture ) {
    defines += "#define OZ_VERTEX_TEXTURE\n";
  }
  if( doEnvMap ) {
    defines += "#define OZ_ENV_MAP\n";
  }
  if( doBumpMap ) {
    defines += "#define OZ_BUMP_MAP\n";
  }
  if( isLowDetail ) {
    defines += "#define OZ_LOW_DETAIL\n";
  }
  if( doPostprocess ) {
    defines += "#define OZ_POSTPROCESS\n";
  }

  File file = "@glsl/header.glsl";
  Buffer buffer = file.read();

  if( buffer.isEmpty() ) {
    OZ_ERROR( "'%s' read failed", file.path().cstr() );
  }

  File dir = "@glsl";
  DArray<File> shaderFiles = dir.ls();

  foreach( file, shaderFiles.citer() ) {
    if( file->hasExtension( "vert" ) ) {
      uint id = glCreateShader( GL_VERTEX_SHADER );

      vertShaders.add( file->baseName(), id );
      compileShader( id, defines, file->path() );
    }
    else if( file->hasExtension( "frag" ) ) {
      uint id = glCreateShader( GL_FRAGMENT_SHADER );

      fragShaders.add( file->baseName(), id );
      compileShader( id, defines, file->path() );
    }
  }

  for( int i = 0; i < liber.shaders.length(); ++i ) {
    loadProgram( i );
  }

  Log::printEnd( " OK" );
}

void Shader::destroy()
{
  Log::print( "Destroying Shader ..." );

  for( int i = 0; i < liber.shaders.length(); ++i ) {
    if( programs[i].program != 0 ) {
      glDetachShader( programs[i].program, programs[i].vertShader );
      glDetachShader( programs[i].program, programs[i].fragShader );
      glDeleteProgram( programs[i].program );
      programs[i].program = 0;
    }
  }
  foreach( vertShader, vertShaders.citer() ) {
    glDeleteShader( vertShader->value );
  }
  foreach( fragShader, fragShaders.citer() ) {
    glDeleteShader( fragShader->value );
  }

  fragShaders.clear();
  fragShaders.deallocate();
  vertShaders.clear();
  vertShaders.deallocate();
  programs.clear();

  if( defaultNormals != 0 ) {
    glDeleteTextures( 1, &defaultNormals );
    defaultNormals = 0;
  }
  if( defaultMasks != 0 ) {
    glDeleteTextures( 1, &defaultMasks );
    defaultMasks = 0;
  }
  if( defaultTexture != 0 ) {
    glDeleteTextures( 1, &defaultTexture );
    defaultTexture = 0;
  }

  plain = -1;
  defines = "";

  OZ_GL_CHECK_ERROR();

  Log::printEnd( " OK" );
}

Shader shader;

}
}
