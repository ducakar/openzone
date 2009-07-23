/*
 *  Context.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#include "precompiled.h"

#include "Context.h"

#include <vorbis/vorbisfile.h>

#include "MD2Model.h"
#include "MD2StaticModel.h"
#include "OBJModel.h"
#include "SimpleAudio.h"

#define OZ_REGISTER_MODELCLASS( name ) \
  modelClasses.add( #name, &name##Model::create )

#define OZ_REGISTER_AUDIOCLASS( name ) \
  audioClasses.add( #name, &name##Audio::create )

namespace oz
{
namespace client
{

  Context context;

  uint Context::buildTexture( const ubyte *data, int width, int height, int bytesPerPixel,
                              bool wrap, int magFilter, int minFilter )
  {
    assert( glGetError() == GL_NO_ERROR );

    GLenum format = bytesPerPixel == 4 ? GL_RGBA : GL_RGB;

    uint texNum;
    glGenTextures( 1, &texNum );
    glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
    glBindTexture( GL_TEXTURE_2D, texNum );

    if( minFilter >= GL_NEAREST_MIPMAP_NEAREST ) {
      gluBuild2DMipmaps( GL_TEXTURE_2D, format, width, height, format,
                         GL_UNSIGNED_BYTE, data );
    }
    else {
      glTexImage2D( GL_TEXTURE_2D, 0, format, width, height, 0, format,
                    GL_UNSIGNED_BYTE, data );
    }

    if( !wrap ) {
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
    }

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter );

//     glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );

    if( glGetError() != GL_NO_ERROR ) {
      glDeleteTextures( 1, &texNum );
      texNum = ~0;

      assert( glGetError() == GL_NO_ERROR );
    }
    return texNum;
  }

  uint Context::buildNormalmap( ubyte *data, const Vec3 &lightNormal, int width, int height,
                                int bytesPerPixel, bool wrap, int magFilter, int minFilter )
  {
    assert( glGetError() == GL_NO_ERROR );

    ubyte *dataEnd = data + width * height * bytesPerPixel;

    for( ubyte *p = data; p < dataEnd; p += bytesPerPixel ) {
      float x = ( (float) p[0] - 128.0f ) / 128.0f;
      float y = ( (float) p[1] - 128.0f ) / 128.0f;
      float z = ( (float) p[2] - 128.0f ) / 128.0f;

      float dot = x * lightNormal.x + y * lightNormal.y + z * lightNormal.z;
      ubyte color = (ubyte) bound( dot * 256.0f, 0.0f, 255.0f );

      p[0] = color;
      p[1] = color;
      p[2] = color;
    }

    GLenum format = bytesPerPixel == 4 ? GL_RGBA : GL_RGB;

    uint texNum;
    glGenTextures( 1, &texNum );
    glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
    glBindTexture( GL_TEXTURE_2D, texNum );

    if( minFilter >= GL_NEAREST_MIPMAP_NEAREST ) {
      gluBuild2DMipmaps( GL_TEXTURE_2D, format, width, height, format,
                         GL_UNSIGNED_BYTE, data );
    }
    else {
      glTexImage2D( GL_TEXTURE_2D, 0, format, width, height, 0, format,
                    GL_UNSIGNED_BYTE, data );
    }

    if( !wrap ) {
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
    }

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter );

//     glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );

    if( glGetError() != GL_NO_ERROR ) {
      glDeleteTextures( 1, &texNum );
      texNum = ~0;

      assert( glGetError() == GL_NO_ERROR );
    }
    return texNum;
  }

  uint Context::createTexture( const ubyte *data, int width, int height, int bytesPerPixel,
                               bool wrap, int magFilter, int minFilter )
  {
    int texNum = buildTexture( data, width, height, bytesPerPixel, wrap, magFilter, minFilter );

    if( texNum == ~0 ) {
      logFile.println( "Error creating texture from buffer" );
    }
    return texNum;
  }

  uint Context::createNormalmap( ubyte *data, const Vec3 &lightNormal, int width, int height,
                                 int bytesPerPixel, bool wrap, int magFilter, int minFilter )
  {
    int texNum = buildNormalmap( data, lightNormal, width, height, bytesPerPixel, wrap,
                                 magFilter, minFilter );

    if( texNum == ~0 ) {
      logFile.println( "Error creating normalmap texture from buffer" );
    }
    return texNum;
  }

  uint Context::requestTexture( int resource, bool wrap, int magFilter, int minFilter )
  {
    if( textures[resource].nUsers >= 0 ) {
      textures[resource].nUsers++;
      return textures[resource].id;
    }
    textures[resource].nUsers = 1;

    String &name = translator.textures[resource].name;
    String &path = translator.textures[resource].path;

    logFile.print( "Loading registered texture '%s' ...", name.cstr() );

    SDL_Surface *image = IMG_Load( path.cstr() );
    if( image == null ) {
      logFile.printEnd( " No such file" );
      return 0;
    }
    logFile.printEnd( " OK" );

    int bytesPerPixel = image->format->BitsPerPixel / 8;
    int texNum = createTexture( (const ubyte*) image->pixels, image->w, image->h,
                                bytesPerPixel, wrap, magFilter, minFilter );

    SDL_FreeSurface( image );

    textures[resource].id = texNum;
    return texNum;
  }

  uint Context::requestNormalmap( int resource, const Vec3 &lightNormal,
                                  bool wrap, int magFilter, int minFilter )
  {
    if( textures[resource].nUsers >= 0 ) {
      textures[resource].nUsers++;
      return textures[resource].id;
    }
    textures[resource].nUsers = 1;

    String &name = translator.textures[resource].name;
    String &path = translator.textures[resource].path;

    logFile.print( "Loading registerded normalmap texture '%s' ...", name.cstr() );

    SDL_Surface *image = IMG_Load( path.cstr() );
    if( image == null ) {
      logFile.printEnd( " No such file" );
      return 0;
    }
    logFile.printEnd( " OK" );

    int bytesPerPixel = image->format->BitsPerPixel / 8;
    int texNum = createNormalmap( (ubyte*) image->pixels, lightNormal, image->w, image->h,
                                  bytesPerPixel, wrap, magFilter, minFilter );

    SDL_FreeSurface( image );

    textures[resource].id = texNum;
    return texNum;
  }

  void Context::releaseTexture( int resource )
  {
    assert( textures[resource].nUsers > 0 );

    textures[resource].nUsers--;

    if( textures[resource].nUsers == 0 ) {
      glDeleteTextures( 1, &textures[resource].id );
      textures[resource].nUsers = -1;
    }
  }

  uint Context::loadTexture( const char *path, bool wrap, int magFilter, int minFilter )
  {
    logFile.print( "Loading texture from file '%s' ...", path );

    SDL_Surface *image = IMG_Load( path );
    if( image == null ) {
      logFile.printEnd( " No such file" );
      return 0;
    }
    logFile.printEnd( " OK" );

    int bytesPerPixel = image->format->BitsPerPixel / 8;
    int texNum = createTexture( (const ubyte*) image->pixels, image->w, image->h,
                                bytesPerPixel, wrap, magFilter, minFilter );

    SDL_FreeSurface( image );

    return texNum;
  }

  uint Context::loadNormalmap( const char *path, const Vec3 &lightNormal,
                               bool wrap, int magFilter, int minFilter )
  {
    logFile.print( "Loading normalmap texture from file '%s' ...", path );

    SDL_Surface *image = IMG_Load( path );
    if( image == null ) {
      logFile.printEnd( " No such file" );
      return 0;
    }
    logFile.printEnd( " OK" );

    int bytesPerPixel = image->format->BitsPerPixel / 8;
    int texNum = createNormalmap( (ubyte*) image->pixels, lightNormal, image->w, image->h,
                                  bytesPerPixel, wrap, magFilter, minFilter );

    SDL_FreeSurface( image );

    return texNum;
  }

  void Context::freeTexture( uint id )
  {
    glDeleteTextures( 1, &id );
  }

  uint Context::requestSound( int resource )
  {
    if( sounds[resource].nUsers >= 0 ) {
      sounds[resource].nUsers++;
      return sounds[resource].id;
    }
    sounds[resource].nUsers = 1;

    const String &path = translator.sounds[resource].path;
    logFile.print( "Loading sound '%s' ...", translator.sounds[resource].name.cstr() );

    int dot = path.lastIndex( '.' );
    if( dot <= 0 ) {
      logFile.printEnd( " Extension missing" );
      return false;
    }
    String extension = path.substring( dot );

    if( String::equals( extension, ".au" ) || String::equals( extension, ".wav" ) ) {
      sounds[resource].id = alutCreateBufferFromFile( path );

      if( sounds[resource].id == AL_NONE ) {
        logFile.printEnd( " Failed" );
        return AL_NONE;
      }
    }
    else if( String::equals( extension, ".oga" ) || String::equals( extension, ".ogg" ) ) {
      // FIXME make this loader work
      FILE           *oggFile = fopen( path, "rb" );
      OggVorbis_File oggStream;
      vorbis_info    *vorbisInfo;
      ALenum         format;

      if( oggFile == null ) {
        logFile.printEnd( " Failed to open file" );
        return false;
      }
      if( ov_open( oggFile, &oggStream, null, 0 ) < 0 ) {
        fclose( oggFile );
        logFile.printEnd( " Failed to open Ogg stream" );
        return false;
      }

      vorbisInfo = ov_info( &oggStream, -1 );
      if( vorbisInfo == null ) {
        ov_clear( &oggStream );
        logFile.printEnd( " Failed to read Vorbis header" );
        return false;
      }

      if( vorbisInfo->channels == 1 ) {
        format = AL_FORMAT_MONO16;
      }
      else if( vorbisInfo->channels == 2 ) {
        format = AL_FORMAT_STEREO16;
      }
      else {
        ov_clear( &oggStream );
        logFile.printEnd( " Invalid number of channels, should be 1 or 2" );
        return AL_NONE;
      }

      int  size = (int) ( oggStream.end - oggStream.offset );
      char data[size];
      int  section;
      int  bytesRead = 0;
      int  result;
      do {
        result = ov_read( &oggStream, &data[bytesRead], size - bytesRead, 0, 2, 1, &section );
        bytesRead += result;
        if( result < 0 ) {
          ov_clear( &oggStream );
          logFile.printEnd( " Failed to decode Vorbis stream, error %d", result );
          return AL_NONE;
        }
      }
      while( result > 0 && bytesRead < size );

      ov_clear( &oggStream );

      alGenBuffers( 1, &sounds[resource].id );
      alBufferData( sounds[resource].id, format, data, bytesRead, vorbisInfo->rate );
      if( alGetError() != AL_NO_ERROR ) {
        logFile.printEnd( " Failed to create buffer" );
        return AL_NONE;
      }
    }
    else {
      logFile.printEnd( " Unknown file extension '%s'", extension.cstr() );
      return AL_NONE;
    }

    logFile.printEnd( " OK" );
    return sounds[resource].id;
  }

  void Context::releaseSound( int resource )
  {
    assert( sounds[resource].nUsers > 0 );

    sounds[resource].nUsers--;
  }

  uint Context::genList()
  {
    int index = lists.add();
    lists[index].base = glGenLists( 1 );
    lists[index].count = 1;
    return lists[index].base;
  }

  uint Context::genLists( int count )
  {
    int index = lists.add();
    lists[index].base = glGenLists( count );
    lists[index].count = count;
    return lists[index].base;
  }

  void Context::freeLists( uint listId )
  {
    for( int i = 0; i < lists.length(); i++ ) {
      if( lists[i].base == listId ) {
        glDeleteLists( lists[i].base, lists[i].count );
      }
    }
  }

  uint Context::loadMD2StaticModel( const char *path )
  {
    if( md2StaticModels.contains( path ) ) {
      md2StaticModels.cachedValue().nUsers++;
      return md2StaticModels.cachedValue().id;
    }
    else {
      md2StaticModels.add( path, Resource<uint>() );
      md2StaticModels.cachedValue().id = MD2::genList( path );
      md2StaticModels.cachedValue().nUsers = 1;
      return md2StaticModels.cachedValue().id;
    }
  }

  MD2 *Context::loadMD2Model( const char *path )
  {
    if( md2Models.contains( path ) ) {
      md2Models.cachedValue().nUsers++;
      return md2Models.cachedValue().object;
    }
    else {
      md2Models.add( path, Resource<MD2*>() );
      md2Models.cachedValue().object = new MD2();
      md2Models.cachedValue().object->load( path );
      md2Models.cachedValue().nUsers = 1;
      return md2Models.cachedValue().object;
    }
  }

  uint Context::loadOBJModel( const char *path )
  {
    if( objModels.contains( path ) ) {
      objModels.cachedValue().nUsers++;
      return objModels.cachedValue().id;
    }
    else {
      objModels.add( path, Resource<uint>() );
      objModels.cachedValue().id = OBJ::genList( path );
      objModels.cachedValue().nUsers = 1;
      return objModels.cachedValue().id;
    }
  }

  Context::Context() : textures( null ), sounds( null )
  {}

  void Context::init()
  {
    logFile.print( "Initializing Context ..." );

    assert( textures == null && sounds == null );

    textures = new Resource<uint>[translator.textures.length()];
    sounds = new Resource<uint>[translator.sounds.length()];

    for( int i = 0; i < translator.textures.length(); i++ ) {
      textures[i].nUsers = -1;
    }
    for( int i = 0; i < translator.sounds.length(); i++ ) {
      sounds[i].nUsers = -1;
    }

    OZ_REGISTER_MODELCLASS( MD2 );
    OZ_REGISTER_MODELCLASS( MD2Static );
    OZ_REGISTER_MODELCLASS( OBJ );

    OZ_REGISTER_AUDIOCLASS( Simple );

    logFile.printEnd( " OK" );
  }

  void Context::free()
  {
    logFile.print( "Clearing Context ..." );

    if( textures != null ) {
      delete[] textures;
      textures = null;
    }
    if( sounds != null ) {
      delete[] sounds;
      sounds = null;
    }

    lists.clear();

    md2Models.clear();
    md2StaticModels.clear();
    md3Models.clear();
    md3StaticModels.clear();
    objModels.clear();

    modelClasses.clear();
    audioClasses.clear();

    logFile.printEnd( " OK" );
  }

}
}
