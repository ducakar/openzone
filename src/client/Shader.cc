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

#include <stable.hh>
#include <client/Shader.hh>

#include <matrix/Liber.hh>

#include <client/Camera.hh>
#include <client/OpenGL.hh>

#define OZ_REGISTER_PARAMETER( paramVar, uniformName ) \
  programs[id].param.paramVar = glGetUniformLocation( programs[id].program, uniformName )

#define OZ_REGISTER_ATTRIBUTE( location, name ) \
  glBindAttribLocation( programs[id].program, location, name )

namespace oz
{
namespace client
{

Param param;

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
  projCamera = proj * camera;

  glUniform3fv( param.oz_CameraPosition, 1, client::camera.p );
}

void Transform::applyModel() const
{
  glUniformMatrix4fv( param.oz_ModelTransform, 1, GL_FALSE, model );
}

void Transform::apply() const
{
  glUniformMatrix4fv( param.oz_ProjModelTransform, 1, GL_FALSE, projCamera * model );
  glUniformMatrix4fv( param.oz_ModelTransform, 1, GL_FALSE, model );
}

void Transform::applyColour() const
{
  setColour( colour );
}

void Transform::setColour( const Mat44& colour_ ) const
{
  glUniformMatrix4fv( param.oz_ColourTransform, 1, GL_FALSE, colour_ );
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

const int Shader::SAMPLER_MAP[] = { 0, 1, 2, 3, 4, 5 };
char      Shader::logBuffer[LOG_BUFFER_SIZE];
String    Shader::defines;

Shader::Light::Light( const Point& pos_, const Vec4& diffuse_ ) :
  pos( pos_ ), diffuse( diffuse_ )
{}

void Shader::compileShader( uint id, const char* path, const char** sources, int* lengths ) const
{
  PFile file( path );
  if( !file.map() ) {
    OZ_ERROR( "Shader source '%s' mmap failed", path );
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
      Log::verboseMode = true;
    }

    Log::printRaw( "\n%s:\n%s", path, logBuffer );
    Log::verboseMode = false;
  }

  if( result != GL_TRUE ) {
    OZ_ERROR( "Shader '%s' compile failed", path );
  }

  OZ_GL_CHECK_ERROR();
}

void Shader::loadProgram( int id )
{
  const String& name = liber.shaders[id].name;

  PFile configFile( "glsl/" + name + ".json" );
  JSON programConfig;

  if( !programConfig.load( &configFile ) ) {
    OZ_ERROR( "Failed to read shader program configuration '%s'", configFile.path().cstr() );
  }

  const char* vertName = programConfig[0].asString();
  const char* fragName = programConfig[1].asString();

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

  programs[id].vertShader = *vertId;
  programs[id].fragShader = *fragId;
  programs[id].program    = glCreateProgram();

  glAttachShader( programs[id].program, programs[id].vertShader );
  glAttachShader( programs[id].program, programs[id].fragShader );

  OZ_REGISTER_ATTRIBUTE( Attrib::POSITION, "inPosition" );
  OZ_REGISTER_ATTRIBUTE( Attrib::TEXCOORD, "inTexCoord" );
  OZ_REGISTER_ATTRIBUTE( Attrib::NORMAL,   "inNormal"   );

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

  OZ_REGISTER_PARAMETER( oz_ProjModelTransform,  "oz_ProjModelTransform"  );
  OZ_REGISTER_PARAMETER( oz_ModelTransform,      "oz_ModelTransform"      );
  OZ_REGISTER_PARAMETER( oz_BoneTransforms,      "oz_BoneTransforms"      );
  OZ_REGISTER_PARAMETER( oz_MeshAnimation,       "oz_MeshAnimation"       );

  OZ_REGISTER_PARAMETER( oz_ColourTransform,     "oz_ColourTransform"     );
  OZ_REGISTER_PARAMETER( oz_Textures,            "oz_Textures"            );

  OZ_REGISTER_PARAMETER( oz_CaelumLight_dir,     "oz_CaelumLight.dir"     );
  OZ_REGISTER_PARAMETER( oz_CaelumLight_diffuse, "oz_CaelumLight.diffuse" );
  OZ_REGISTER_PARAMETER( oz_CaelumLight_ambient, "oz_CaelumLight.ambient" );
  OZ_REGISTER_PARAMETER( oz_CameraPosition,      "oz_CameraPosition"      );

  OZ_REGISTER_PARAMETER( oz_Fog_dist,            "oz_Fog.dist"            );
  OZ_REGISTER_PARAMETER( oz_Fog_colour,          "oz_Fog.colour"          );

  OZ_REGISTER_PARAMETER( oz_StarsColour,         "oz_StarsColour"         );
  OZ_REGISTER_PARAMETER( oz_WaveBias,            "oz_WaveBias"            );
  OZ_REGISTER_PARAMETER( oz_Wind,                "oz_Wind"                );

  param = programs[id].param;

  if( setSamplerMap ) {
    glUniform1iv( param.oz_Textures, aLength( SAMPLER_MAP ), SAMPLER_MAP );
  }

  Mat44 bones[] = {
    Mat44::ID, Mat44::ID, Mat44::ID, Mat44::ID,
    Mat44::ID, Mat44::ID, Mat44::ID, Mat44::ID,
    Mat44::ID, Mat44::ID, Mat44::ID, Mat44::ID,
    Mat44::ID, Mat44::ID, Mat44::ID, Mat44::ID
  };

  glUniformMatrix4fv( param.oz_BoneTransforms, 16, GL_FALSE, bones[0] );

  OZ_GL_CHECK_ERROR();
}

Shader::Shader() :
  mode( UI ), plain( -1 ), defaultMasks( 0 )
{}

void Shader::program( int id )
{
  if( id == activeProgram ) {
    return;
  }

  activeProgram = id;

  glUseProgram( programs[id].program );
  param = programs[id].param;

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
  glUniform3fv( param.oz_CaelumLight_dir,     1, caelumLight.dir );
  glUniform4fv( param.oz_CaelumLight_diffuse, 1, caelumLight.diffuse );
  glUniform4fv( param.oz_CaelumLight_ambient, 1, caelumLight.ambient );
}

void Shader::init()
{
  Log::print( "Initialising Shader ..." );

  hasVertexTexture = config.include( "shader.vertexTexture", true ).asBool();
  setSamplerMap    = config.include( "shader.setSamplerMap", true ).asBool();
  doPostprocess    = config["render.postprocess"].asBool();
  isLowDetail      = config["render.lowDetail"].asBool();

  medium           = 0;

  // bind white texture to id 0 to emulate fixed functionality (in fixed functionality sampler
  // always returns white colour when texture 0 is bound)
  ubyte whitePixel[] = { 0xff, 0xff, 0xff, 0xff };

  glGenTextures( 1, &defaultTexture );
  glBindTexture( GL_TEXTURE_2D, defaultTexture );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
  glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, whitePixel );

  // default masks (specular 0.0, emission 0.0)
  ubyte masksPixel[] = { 0x00, 0x00, 0x00, 0xff };

  glGenTextures( 1, &defaultMasks );
  glBindTexture( GL_TEXTURE_2D, defaultMasks );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
  glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, masksPixel );

  // default normals for bumpmap [0, 0, 1]
  ubyte normalsPixel[] = { 0x80, 0x80, 0xff, 0xff };

  glGenTextures( 1, &defaultNormals );
  glBindTexture( GL_TEXTURE_2D, defaultNormals );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
  glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, normalsPixel );

  glBindTexture( GL_TEXTURE_2D, shader.defaultTexture );

  for( int i = 2; i >= 0; --i ) {
    glActiveTexture( GL_TEXTURE0 + uint( i ) );
    glBindTexture( GL_TEXTURE_2D, shader.defaultTexture );
  }

  if( liber.shaders.length() == 0 ) {
    OZ_ERROR( "Shaders missing" );
  }

  plain       = liber.shaderIndex( "plain" );
  mesh        = liber.shaderIndex( "mesh" );
  postprocess = liber.shaderIndex( "postprocess" );

  programs.resize( liber.shaders.length() );

  const char* sources[3];
  int         lengths[3];

#ifdef GL_ES_VERSION_2_0
  defines = "#version 100\n";
#else
  defines = "#version 120\n";
#endif
  defines = defines + ( hasVertexTexture ? "#define OZ_VERTEX_TEXTURE\n" : "\n" );
  defines = defines + ( doPostprocess ?    "#define OZ_POSTPROCESS\n" : "\n" );
  defines = defines + ( isLowDetail ?      "#define OZ_LOW_DETAIL\n" : "\n" );

  for( int i = 4; i < 10; ++i ) {
    defines = defines + "\n";
  }

  sources[0] = defines;
  lengths[0] = defines.length();

  Buffer buffer = PFile( "glsl/header.glsl" ).read();
  if( buffer.isEmpty() ) {
    OZ_ERROR( "header.glsl reading failed" );
  }

  sources[1] = buffer.begin();
  lengths[1] = buffer.length();

  PFile dir( "glsl" );
  DArray<PFile> shaderFiles = dir.ls();

  foreach( file, shaderFiles.citer() ) {
    if( file->hasExtension( "vert" ) ) {
      uint id = glCreateShader( GL_VERTEX_SHADER );

      vertShaders.add( file->baseName(), id );
      compileShader( id, file->path(), sources, lengths );
    }
    else if( file->hasExtension( "frag" ) ) {
      uint id = glCreateShader( GL_FRAGMENT_SHADER );

      fragShaders.add( file->baseName(), id );
      compileShader( id, file->path(), sources, lengths );
    }
  }

  for( int i = 0; i < liber.shaders.length(); ++i ) {
    loadProgram( i );
  }

  Log::printEnd( " OK" );
}

void Shader::free()
{
  Log::print( "Freeing Shader ..." );

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
