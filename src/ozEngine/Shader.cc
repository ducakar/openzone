/*
 * ozEngine - OpenZone Engine Library.
 *
 * Copyright © 2002-2013 Davorin Učakar
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
 * @file ozEngine/Shader.cc
 */

#include "Shader.hh"

#include "GL.hh"

#define OZ_REGISTER_UNIFORM( uniformVar, uniformName ) \
  programs[id].uniform.uniformVar = glGetUniformLocation( programs[id].program, uniformName )

#define OZ_REGISTER_ATTRIBUTE( location, name ) \
  glBindAttribLocation( programs[id].program, location, name )

namespace oz
{

struct Program
{
  GLuint  vertShader;
  GLuint  fragShader;
  GLuint  program;
  Uniform uniform;
};

static Map<String, uint> vertShaders;
static Map<String, uint> fragShaders;
static List<Program>     programs;
#if 0
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

  glUniform3fv( uniform.cameraPosition, 1, client::camera.p );
}

void Transform::applyModel() const
{
  glUniformMatrix4fv( uniform.modelTransform, 1, GL_FALSE, model );
}

void Transform::apply() const
{
  glUniformMatrix4fv( uniform.projModelTransform, 1, GL_FALSE, projCamera * model );
  glUniformMatrix4fv( uniform.modelTransform, 1, GL_FALSE, model );
}

void Transform::applyColour() const
{
  setColour( colour );
}

void Transform::setColour( const Mat44& colour_ ) const
{
  glUniformMatrix4fv( uniform.colourTransform, 1, GL_FALSE, colour_ );
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
#endif
const int Shader::SAMPLER_MAP[] = { 0, 1, 2, 3, 4, 5 };
char      Shader::logBuffer[LOG_BUFFER_SIZE];
String    Shader::defines;
#if 0
Shader::Light::Light( const Point& pos_, const Vec4& diffuse_ ) :
  pos( pos_ ), diffuse( diffuse_ )
{}
#endif

static Buffer shaderPrologue;

bool Shader::readFile( const File& file, OutputStream* ostream, List<String>* fileNames,
                       List<int>* fileLengths )
{
  Buffer buffer = file.read();
  if( buffer.isEmpty() ) {
    return false;
  }

  InputStream is    = buffer.inputStream();
  int         begin = ostream->tell();

  while( is.isAvailable() ) {
    String line = is.readLine();

    if( line.beginsWith( "#include" ) ) {
      int startQuote = line.index( '"' );
      int endQuote   = line.lastIndex( '"' );

      if( startQuote > 0 && startQuote < endQuote ) {
        File includeFile = file.directory() + "/" + line.substring( startQuote + 1, endQuote );

        if( !readFile( includeFile, ostream, fileNames, fileLengths ) ) {
          return false;
        }
      }
    }
    else {
      ostream->writeLine( line );
    }
  }

  if( !fileNames->contains( file.path() ) ) {
    fileNames->add( file.path() );
    fileLengths->add( ostream->tell() - begin );
  }
  return true;
}

bool Shader::loadShader( const File& file, GLenum type )
{
  List<String> fileNames;
  List<int>    fileOffsets;
  OutputStream ostream( 0 );

  fileNames.add();
  ostream.writeChars( shaderPrologue.begin(), shaderPrologue.length() );

  if( !readFile( file, &ostream, &fileNames, &fileOffsets ) ) {
    return false;
  }

  const char** fileContents = new const char*[ fileNames.length() ];
  int          fileLengths  = new int[ fileNames.length() ];
  int          prevOffset   = 0;

  for( int i = 0; i < fileOffsets.length(); ++i ) {
    fileContents[i] = ostream.begin() + fileOffsets[i];
    fileLengths[i]  = i == fileOffsets.length() - 1 ?  :
                                                      fileOffsets[i + 1] - fileOffsets[i];
  }

  hard_assert( fileNames.length() == fileContents.length() &&
               fileNames.length() == fileLengths.length() );

  GLuint id = glCreateShader( type );
  glShaderSource( id, fileNames.length(), fileContents.fi, fileLengths.begin() );
  glCompileShader( id );

  int result, length;

  glGetShaderiv( id, GL_COMPILE_STATUS, &result );
  glGetShaderInfoLog( id, LOG_BUFFER_SIZE - 1, &length, logBuffer );
  logBuffer[LOG_BUFFER_SIZE - 1] = '\0';

  if( length != 0 ) {
    Log::printRaw( "\n%s:\n%s", file.path().cstr(), logBuffer );
  }

  OZ_GL_CHECK_ERROR();

  return result == GL_TRUE;
}

void Shader::loadProgram( const char* name )
{
  File configFile( String( "@glsl/%s.json", name ) );
  JSON programConfig;

  if( !programConfig.load( configFile ) ) {
    OZ_ERROR( "Failed to read shader program configuration '%s'", configFile.path().cstr() );
  }

  const char* vertName = programConfig[0].asString();
  const char* fragName = programConfig[1].asString();

  const uint* vertId = vertShaders.find( vertName );
  const uint* fragId = fragShaders.find( fragName );

  if( vertId == nullptr ) {
    OZ_ERROR( "Invalid vertex shader '%s' requested for shader program '%s'", vertName, name );
  }
  if( fragId == nullptr ) {
    OZ_ERROR( "Invalid fragment shader '%s' requested for shader program '%s'", fragName, name );
  }

  int id = programs.length();

  programs.add();
  programs[id].vertShader = *vertId;
  programs[id].fragShader = *fragId;
  programs[id].program    = glCreateProgram();

  glAttachShader( programs[id].program, programs[id].vertShader );
  glAttachShader( programs[id].program, programs[id].fragShader );

  OZ_REGISTER_ATTRIBUTE( POSITION, "inPosition" );
  OZ_REGISTER_ATTRIBUTE( TEXCOORD, "inTexCoord" );
  OZ_REGISTER_ATTRIBUTE( NORMAL,   "inNormal"   );

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

    Log::printRaw( "\n%s:\n%s", name, logBuffer );
    Log::verboseMode = false;
  }

  if( result != GL_TRUE ) {
    OZ_ERROR( "Shader program '%s' linking failed", name );
  }

  glUseProgram( programs[id].program );

  OZ_REGISTER_UNIFORM( projModelTransform,  "oz_ProjModelTransform"  );
  OZ_REGISTER_UNIFORM( modelTransform,      "oz_ModelTransform"      );
  OZ_REGISTER_UNIFORM( boneTransforms,      "oz_BoneTransforms"      );
  OZ_REGISTER_UNIFORM( meshAnimation,       "oz_MeshAnimation"       );

  OZ_REGISTER_UNIFORM( colourTransform,     "oz_ColourTransform"     );
  OZ_REGISTER_UNIFORM( textures,            "oz_Textures"            );

  OZ_REGISTER_UNIFORM( caelumLight_dir,     "oz_CaelumLight.dir"     );
  OZ_REGISTER_UNIFORM( caelumLight_diffuse, "oz_CaelumLight.diffuse" );
  OZ_REGISTER_UNIFORM( caelumLight_ambient, "oz_CaelumLight.ambient" );
  OZ_REGISTER_UNIFORM( cameraPosition,      "oz_CameraPosition"      );

  OZ_REGISTER_UNIFORM( fog_dist,            "oz_Fog.dist"            );
  OZ_REGISTER_UNIFORM( fog_colour,          "oz_Fog.colour"          );

  OZ_REGISTER_UNIFORM( starsColour,         "oz_StarsColour"         );
  OZ_REGISTER_UNIFORM( waveBias,            "oz_WaveBias"            );
  OZ_REGISTER_UNIFORM( wind,                "oz_Wind"                );

//   uniform = programs[id].uniform;

  if( setSamplerMap ) {
//     glUniform1iv( uniform.textures, aLength( SAMPLER_MAP ), SAMPLER_MAP );
  }

  Mat44 bones[] = {
    Mat44::ID, Mat44::ID, Mat44::ID, Mat44::ID,
    Mat44::ID, Mat44::ID, Mat44::ID, Mat44::ID,
    Mat44::ID, Mat44::ID, Mat44::ID, Mat44::ID,
    Mat44::ID, Mat44::ID, Mat44::ID, Mat44::ID
  };

//   glUniformMatrix4fv( uniform.boneTransforms, 16, GL_FALSE, bones[0] );

  OZ_GL_CHECK_ERROR();
}

Shader::Shader() :
  mode( UI ), plain( -1 ), defaultMasks( 0 )
{}
#if 0
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
  glUniform4fv( uniform.caelumLight_diffuse, 1, caelumLight.diffuse );
  glUniform4fv( uniform.caelumLight_ambient, 1, caelumLight.ambient );
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

  File file( "@glsl/header.glsl" );
  Buffer buffer = file.read();

  if( buffer.isEmpty() ) {
    OZ_ERROR( "'%s' read failed", file.path().cstr() );
  }

  sources[1] = buffer.begin();
  lengths[1] = buffer.length();

  File dir( "@glsl" );
  DArray<File> shaderFiles = dir.ls();

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
#endif
#if 0
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
#endif
}
