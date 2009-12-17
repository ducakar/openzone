/*
 *  Context.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3.0. See COPYING for details.
 */

#include "precompiled.h"

#include "Context.h"

#include "MD2.h"
#include "MD3.h"
#include "OBJ.h"

#include "OBJModel.h"
#include "ExplosionModel.h"
#include "MD2StaticModel.h"
#include "MD2Model.h"
#include "MD2WeaponModel.h"
#include "MD3StaticModel.h"
#include "OBJVehicleModel.h"

#include "BasicAudio.h"

#include <SDL_image.h>
#include <GL/glu.h>
#include <AL/alut.h>
#include <vorbis/vorbisfile.h>

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
      float x = static_cast<float>( p[0] - 128.0f ) / 128.0f;
      float y = static_cast<float>( p[1] - 128.0f ) / 128.0f;
      float z = static_cast<float>( p[2] - 128.0f ) / 128.0f;

      float dot = x * lightNormal.x + y * lightNormal.y + z * lightNormal.z;
      ubyte color = static_cast<ubyte>( bound( dot * 256.0f, 0.0f, 255.0f ) );

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
      log.println( "Error creating texture from buffer" );
    }
    return texNum;
  }

  uint Context::createNormalmap( ubyte *data, const Vec3 &lightNormal, int width, int height,
                                 int bytesPerPixel, bool wrap, int magFilter, int minFilter )
  {
    int texNum = buildNormalmap( data, lightNormal, width, height, bytesPerPixel, wrap,
                                 magFilter, minFilter );

    if( texNum == ~0 ) {
      log.println( "Error creating normalmap texture from buffer" );
    }
    return texNum;
  }

  uint Context::requestTexture( int resource, bool wrap, int magFilter, int minFilter )
  {
    if( textures[resource].nUsers > 0 ) {
      textures[resource].nUsers++;
      return textures[resource].id;
    }
    textures[resource].nUsers = 1;

    String &name = translator.textures[resource].name;
    String &path = translator.textures[resource].path;

    log.print( "Loading registered texture '%s' ...", name.cstr() );

    SDL_Surface *image = IMG_Load( path.cstr() );
    if( image == null ) {
      log.printEnd( " No such file" );
      return 0;
    }
    if( image->format->BitsPerPixel != 24 && image->format->BitsPerPixel != 32 ) {
      log.printEnd( " Wrong format. Should be 24 bpp RGB or 32 bpp RGBA" );
      return 0;
    }
    log.printEnd( " OK" );

    int bytesPerPixel = image->format->BitsPerPixel / 8;
    int texNum = createTexture( reinterpret_cast<const ubyte*>( image->pixels ), image->w, image->h,
                                bytesPerPixel, wrap, magFilter, minFilter );

    SDL_FreeSurface( image );

    textures[resource].id = texNum;
    return texNum;
  }

  uint Context::requestNormalmap( int resource, const Vec3 &lightNormal,
                                  bool wrap, int magFilter, int minFilter )
  {
    if( textures[resource].nUsers > 0 ) {
      textures[resource].nUsers++;
      return textures[resource].id;
    }
    textures[resource].nUsers = 1;

    String &name = translator.textures[resource].name;
    String &path = translator.textures[resource].path;

    log.print( "Loading registerded normalmap texture '%s' ...", name.cstr() );

    SDL_Surface *image = IMG_Load( path.cstr() );
    if( image == null ) {
      log.printEnd( " No such file" );
      return 0;
    }
    if( image->format->BitsPerPixel != 24 && image->format->BitsPerPixel != 32 ) {
      log.printEnd( " Wrong format. Should be 24 bpp RGB or 32 bpp RGBA" );
      return 0;
    }
    log.printEnd( " OK" );

    int bytesPerPixel = image->format->BitsPerPixel / 8;
    int texNum = createNormalmap( reinterpret_cast<ubyte*>( image->pixels ), lightNormal,
                                  image->w, image->h, bytesPerPixel, wrap, magFilter, minFilter );

    SDL_FreeSurface( image );

    textures[resource].id = texNum;
    return texNum;
  }

  void Context::releaseTexture( int resource )
  {
    assert( 0 <= resource && resource < translator.textures.length() );
    assert( textures[resource].nUsers > 0 );

    textures[resource].nUsers--;

    if( textures[resource].nUsers == 0 ) {
      log.print( "Unloading texture '%s' ...", translator.textures[resource].name.cstr() );
      glDeleteTextures( 1, &textures[resource].id );
      log.printEnd( " OK" );
    }
  }

  uint Context::loadTexture( const char *path, bool wrap, int magFilter, int minFilter )
  {
    log.print( "Loading texture from file '%s' ...", path );

    SDL_Surface *image = IMG_Load( path );
    if( image == null ) {
      log.printEnd( " No such file" );
      return 0;
    }
    if( image->format->BitsPerPixel != 24 && image->format->BitsPerPixel != 32 ) {
      log.printEnd( " Wrong format. Should be 24 bpp RGB or 32 bpp RGBA" );
      return 0;
    }
    log.printEnd( " OK" );

    int bytesPerPixel = image->format->BitsPerPixel / 8;
    int texNum = createTexture( reinterpret_cast<const ubyte*>( image->pixels ), image->w, image->h,
                                bytesPerPixel, wrap, magFilter, minFilter );

    SDL_FreeSurface( image );

    return texNum;
  }

  uint Context::loadNormalmap( const char *path, const Vec3 &lightNormal,
                               bool wrap, int magFilter, int minFilter )
  {
    log.print( "Loading normalmap texture from file '%s' ...", path );

    SDL_Surface *image = IMG_Load( path );
    if( image == null ) {
      log.printEnd( " No such file" );
      return 0;
    }
    if( image->format->BitsPerPixel != 24 && image->format->BitsPerPixel != 32 ) {
      log.printEnd( " Wrong format. Should be 24 bpp RGB or 32 bpp RGBA" );
      return 0;
    }
    log.printEnd( " OK" );

    int bytesPerPixel = image->format->BitsPerPixel / 8;
    int texNum = createNormalmap( reinterpret_cast<ubyte*>( image->pixels ), lightNormal,
                                  image->w, image->h, bytesPerPixel, wrap, magFilter, minFilter );

    SDL_FreeSurface( image );
    return texNum;
  }

  void Context::freeTexture( uint id )
  {
    glDeleteTextures( 1, &id );
  }

  uint Context::requestSound( int resource )
  {
    assert( alGetError() == AL_NO_ERROR );

    if( sounds[resource].nUsers == -2 ) {
      sounds[resource].nUsers = 1;
      return sounds[resource].id;
    }
    if( sounds[resource].nUsers >= 0 ) {
      sounds[resource].nUsers++;
      return sounds[resource].id;
    }
    sounds[resource].nUsers = 1;

    const String &path = translator.sounds[resource].path;
    log.print( "Loading sound '%s' ...", translator.sounds[resource].name.cstr() );

    int dot = path.lastIndex( '.' );
    if( dot <= 0 ) {
      log.printEnd( " Extension missing" );
      return AL_NONE;
    }
    String extension = path.substring( dot );

    if( String::equals( extension, ".au" ) || String::equals( extension, ".wav" ) ) {
      sounds[resource].id = alutCreateBufferFromFile( path );

      if( sounds[resource].id == AL_NONE ) {
        log.printEnd( " Failed" );
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
        log.printEnd( " Failed to open file" );
        return AL_NONE;
      }
      if( ov_open( oggFile, &oggStream, null, 0 ) < 0 ) {
        fclose( oggFile );
        log.printEnd( " Failed to open Ogg stream" );
        return AL_NONE;
      }

      vorbisInfo = ov_info( &oggStream, -1 );
      if( vorbisInfo == null ) {
        ov_clear( &oggStream );
        log.printEnd( " Failed to read Vorbis header" );
        return AL_NONE;
      }

      if( vorbisInfo->channels == 1 ) {
        format = AL_FORMAT_MONO16;
      }
      else if( vorbisInfo->channels == 2 ) {
        format = AL_FORMAT_STEREO16;
      }
      else {
        ov_clear( &oggStream );
        log.printEnd( " Invalid number of channels, should be 1 or 2" );
        return AL_NONE;
      }

      int  size = static_cast<int>( oggStream.end - oggStream.offset );
      char data[size];
      int  section;
      int  bytesRead = 0;
      int  result;
      do {
        result = ov_read( &oggStream, &data[bytesRead], size - bytesRead, 0, 2, 1, &section );
        bytesRead += result;
        if( result < 0 ) {
          ov_clear( &oggStream );
          log.printEnd( " Failed to decode Vorbis stream, error %d", result );
          return AL_NONE;
        }
      }
      while( result > 0 && bytesRead < size );

      ov_clear( &oggStream );

      alGenBuffers( 1, &sounds[resource].id );
      alBufferData( sounds[resource].id, format, data, bytesRead, vorbisInfo->rate );
      if( alGetError() != AL_NO_ERROR ) {
        log.printEnd( " Failed to create buffer" );
        return AL_NONE;
      }
    }
    else {
      log.printEnd( " Unknown file extension '%s'", extension.cstr() );
      return AL_NONE;
    }

    assert( alGetError() == AL_NO_ERROR );

    log.printEnd( " OK" );
    return sounds[resource].id;
  }

  void Context::releaseSound( int resource )
  {
    assert( 0 <= resource && resource < translator.sounds.length() );
    assert( sounds[resource].nUsers > 0 );

    sounds[resource].nUsers--;
  }

  void Context::freeSound( int resource )
  {
    assert( 0 <= resource && resource < translator.sounds.length() );
    assert( sounds[resource].nUsers == -2 );

    log.print( "Unloading sound '%s' ...", translator.sounds[resource].name.cstr() );
    alDeleteBuffers( 1, &context.sounds[resource].id );
    context.sounds[resource].nUsers = -1;
    log.printEnd( " OK" );

    assert( alGetError() == AL_NO_ERROR );
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

  uint Context::loadOBJ( const char *path )
  {
    if( !objs.contains( path ) ) {
      objs.add( path, Resource<OBJ*>() );

      OBJ *&obj = objs.cachedValue().object;
      obj = new OBJ( path );
      obj->genList();
      obj->trim();
    }
    objs.cachedValue().nUsers++;
    return objs.cachedValue().object->list;
  }

  void Context::releaseOBJ( const char *path )
  {
    assert( objs.contains( path ) );

    if( objs.contains( path ) ) {
      objs.cachedValue().nUsers--;

      if( objs.cachedValue().nUsers == 0 ) {
        glDeleteLists( objs.cachedValue().object->list, 1 );
        delete objs.cachedValue().object;
        objs.remove( path );
      }
    }
  }

  uint Context::loadStaticMD2( const char *path )
  {
    if( !staticMd2s.contains( path ) ) {
      staticMd2s.add( path, Resource<MD2*>() );

      MD2 *&md2 = staticMd2s.cachedValue().object;
      md2 = new MD2( path );
      md2->genList();
    }
    staticMd2s.cachedValue().nUsers++;
    return staticMd2s.cachedValue().object->list;
  }

  void Context::releaseStaticMD2( const char *path )
  {
    assert( staticMd2s.contains( path ) );

    if( staticMd2s.contains( path ) ) {
      staticMd2s.cachedValue().nUsers--;

      if( staticMd2s.cachedValue().nUsers == 0 ) {
        glDeleteLists( staticMd2s.cachedValue().object->list, 1 );
        delete staticMd2s.cachedValue().object;
        staticMd2s.remove( path );
      }
    }
  }

  MD2 *Context::loadMD2( const char *path )
  {
    if( !md2s.contains( path ) ) {
      md2s.add( path, Resource<MD2*>() );
      md2s.cachedValue().object = new MD2( path );
    }
    md2s.cachedValue().nUsers++;
    return md2s.cachedValue().object;
  }

  void Context::releaseMD2( const char *path )
  {
    assert( md2s.contains( path ) );

    if( md2s.contains( path ) ) {
      md2s.cachedValue().nUsers--;

      if( md2s.cachedValue().nUsers == 0 ) {
        delete md2s.cachedValue().object;
        md2s.remove( path );
      }
    }
  }

  uint Context::loadStaticMD3( const char *path )
  {
    if( !staticMd3s.contains( path ) ) {
      staticMd3s.add( path, Resource<MD3*>() );

      MD3 *&md3 = staticMd3s.cachedValue().object;
      md3 = new MD3( path );
      md3->genList();
      md3->trim();
    }
    staticMd3s.cachedValue().nUsers++;
    return staticMd3s.cachedValue().object->list;
  }

  void Context::releaseStaticMD3( const char *path )
  {
    assert( staticMd3s.contains( path ) );

    if( staticMd3s.contains( path ) ) {
      staticMd3s.cachedValue().nUsers--;

      if( staticMd3s.cachedValue().nUsers == 0 ) {
        glDeleteLists( staticMd3s.cachedValue().object->list, 1 );
        delete staticMd3s.cachedValue().object;
        staticMd3s.remove( path );
      }
    }
  }

  MD3 *Context::loadMD3( const char *path )
  {
    if( !md3s.contains( path ) ) {
      md3s.add( path, Resource<MD3*>() );
      md3s.cachedValue().object = new MD3( path );
    }
    md3s.cachedValue().nUsers++;
    return md3s.cachedValue().object;
  }

  void Context::releaseMD3( const char *path )
  {
    assert( md3s.contains( path ) );

    if( md3s.contains( path ) ) {
      md3s.cachedValue().nUsers--;

      if( md3s.cachedValue().nUsers == 0 ) {
        delete md3s.cachedValue().object;
        md3s.remove( path );
      }
    }
  }

  Context::Context() : textures( null ), sounds( null )
  {}

  void Context::init()
  {
    log.print( "Initializing Context ..." );

    assert( textures == null && sounds == null );

    textures = new Resource<uint>[translator.textures.length()];
    sounds = new Resource<uint>[translator.sounds.length()];

    for( int i = 0; i < translator.sounds.length(); i++ ) {
      sounds[i].id = AL_NONE;
      sounds[i].nUsers = -1;
    }

    OZ_REGISTER_MODELCLASS( OBJ );
    OZ_REGISTER_MODELCLASS( Explosion );
    OZ_REGISTER_MODELCLASS( MD2Static );
    OZ_REGISTER_MODELCLASS( MD2 );
    OZ_REGISTER_MODELCLASS( MD2Weapon );
    OZ_REGISTER_MODELCLASS( MD3Static );
    OZ_REGISTER_MODELCLASS( OBJVehicle );

    OZ_REGISTER_AUDIOCLASS( Basic );

    log.printEnd( " OK" );
  }

  void Context::free()
  {
    log.print( "Clearing Context ..." );

    if( textures != null ) {
      delete[] textures;
      textures = null;
    }
    if( sounds != null ) {
      for( int i = 0; i < translator.sounds.length(); i++ ) {
        if( sounds[i].id != AL_NONE ) {
          alDeleteBuffers( 1, &sounds[i].id );
        }
      }
      delete[] sounds;
      sounds = null;
    }

    md2s.clear();
    staticMd2s.clear();
    md3s.clear();
    staticMd3s.clear();
    objs.clear();

    modelClasses.clear();
    audioClasses.clear();

    log.printEnd( " OK" );
  }

}
}
