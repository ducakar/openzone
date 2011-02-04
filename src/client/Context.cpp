/*
 *  Context.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "client/Context.hpp"

#include "client/MD2.hpp"
#include "client/MD3.hpp"
#include "client/OBJ.hpp"

#include "client/OBJModel.hpp"
#include "client/ExplosionModel.hpp"
#include "client/MD2StaticModel.hpp"
#include "client/MD2Model.hpp"
#include "client/MD2WeaponModel.hpp"
#include "client/MD3StaticModel.hpp"
#include "client/OBJVehicleModel.hpp"

#include "client/BasicAudio.hpp"
#include "client/BotAudio.hpp"

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

  Pool<Context::Source> Context::Source::pool;

  uint Context::buildTexture( const void* data, int width, int height, int bytesPerPixel,
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
      texNum = ~0u;

      assert( glGetError() == GL_NO_ERROR );
    }
    return texNum;
  }

  uint Context::buildNormalmap( void* data_, const Vec3& lightNormal, int width, int height,
                                int bytesPerPixel, bool wrap, int magFilter, int minFilter )
  {
    assert( glGetError() == GL_NO_ERROR );

    ubyte* data    = reinterpret_cast<ubyte*>( data_ );
    ubyte* dataEnd = data + width * height * bytesPerPixel;

    for( ubyte* p = data; p < dataEnd; p += bytesPerPixel ) {
      float x = ( float( p[0] ) - 128.0f ) / 128.0f;
      float y = ( float( p[1] ) - 128.0f ) / 128.0f;
      float z = ( float( p[2] ) - 128.0f ) / 128.0f;

      float dot = x * lightNormal.x + y * lightNormal.y + z * lightNormal.z;
      ubyte colour = ubyte( Math::bound( dot * 256.0f, 0.0f, 255.0f ) );

      p[0] = colour;
      p[1] = colour;
      p[2] = colour;
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
      texNum = ~0u;

      assert( glGetError() == GL_NO_ERROR );
    }
    return texNum;
  }

  uint Context::createTexture( const void* data, int width, int height, int bytesPerPixel,
                               bool wrap, int magFilter, int minFilter )
  {
    uint texNum = buildTexture( data, width, height, bytesPerPixel, wrap, magFilter, minFilter );

    if( texNum == ~0u ) {
      log.println( "Error creating texture from buffer" );
    }
    return texNum;
  }

  uint Context::createNormalmap( void* data, const Vec3& lightNormal, int width, int height,
                                 int bytesPerPixel, bool wrap, int magFilter, int minFilter )
  {
    uint texNum = buildNormalmap( data, lightNormal, width, height, bytesPerPixel, wrap,
                                  magFilter, minFilter );

    if( texNum == ~0u ) {
      log.println( "Error creating normalmap texture from buffer" );
    }
    return texNum;
  }

  uint Context::requestTexture( int id, bool wrap, int magFilter, int minFilter )
  {
    Resource<uint>& resource = textures[id];

    if( resource.nUsers > 0 ) {
      ++resource.nUsers;
      return resource.id;
    }
    resource.nUsers = 1;

    String& name = translator.textures[id].name;
    String& path = translator.textures[id].path;

    log.print( "Loading registered texture '%s' ...", name.cstr() );

    SDL_Surface* image = IMG_Load( path.cstr() );
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
    int texNum = createTexture( image->pixels, image->w, image->h, bytesPerPixel, wrap,
                                magFilter, minFilter );

    SDL_FreeSurface( image );

    resource.id = texNum;
    return texNum;
  }

  uint Context::requestNormalmap( int id, const Vec3& lightNormal,
                                  bool wrap, int magFilter, int minFilter )
  {
    Resource<uint>& resource = textures[id];

    if( resource.nUsers > 0 ) {
      ++resource.nUsers;
      return resource.id;
    }
    resource.nUsers = 1;

    String& name = translator.textures[id].name;
    String& path = translator.textures[id].path;

    log.print( "Loading registerded normalmap texture '%s' ...", name.cstr() );

    SDL_Surface* image = IMG_Load( path.cstr() );
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
    int texNum = createNormalmap( image->pixels, lightNormal, image->w, image->h, bytesPerPixel,
                                  wrap, magFilter, minFilter );

    SDL_FreeSurface( image );

    resource.id = texNum;
    return texNum;
  }

  void Context::releaseTexture( int id )
  {
    Resource<uint>& resource = textures[id];

    assert( uint( id ) < uint( translator.textures.length() ) );
    assert( resource.nUsers > 0 );

    --resource.nUsers;

    if( resource.nUsers == 0 ) {
      log.print( "Unloading texture '%s' ...", translator.textures[id].name.cstr() );
      glDeleteTextures( 1, &resource.id );
      assert( glGetError() == GL_NO_ERROR );
      log.printEnd( " OK" );
    }
  }

  uint Context::loadTexture( const char* path, bool wrap, int magFilter, int minFilter )
  {
    log.print( "Loading texture from file '%s' ...", path );

    SDL_Surface* image = IMG_Load( path );
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
    int texNum = createTexture( image->pixels, image->w, image->h, bytesPerPixel, wrap,
                                magFilter, minFilter );

    SDL_FreeSurface( image );

    return texNum;
  }

  uint Context::loadNormalmap( const char* path, const Vec3& lightNormal,
                               bool wrap, int magFilter, int minFilter )
  {
    log.print( "Loading normalmap texture from file '%s' ...", path );

    SDL_Surface* image = IMG_Load( path );
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
    int texNum = createNormalmap( image->pixels, lightNormal, image->w, image->h, bytesPerPixel,
                                  wrap, magFilter, minFilter );

    SDL_FreeSurface( image );
    return texNum;
  }

  void Context::freeTexture( uint id )
  {
    glDeleteTextures( 1, &id );
    assert( glGetError() == GL_NO_ERROR );
  }

  uint Context::requestSound( int id )
  {
    Resource<uint>& resource = sounds[id];

    if( resource.nUsers >= 0 ) {
      ++resource.nUsers;
      return resource.id;
    }

    resource.nUsers = 1;

    if( resource.nUsers == -2 ) {
      return resource.id;
    }

    assert( alGetError() == AL_NO_ERROR );

    const String& path = translator.sounds[id].path;
    log.print( "Loading sound '%s' ...", translator.sounds[id].name.cstr() );

    int dot = path.lastIndex( '.' );
    if( dot <= 0 ) {
      log.printEnd( " Extension missing" );
      return AL_NONE;
    }
    String extension = path.substring( dot );

    if( extension.equals( ".au" ) || extension.equals( ".wav" ) ) {
      resource.id = alutCreateBufferFromFile( path );

      if( resource.id == AL_NONE ) {
        log.printEnd( " Failed" );
        return AL_NONE;
      }
    }
    else if( extension.equals( ".oga" ) || extension.equals( ".ogg" ) ) {
      FILE*          oggFile = fopen( path, "rb" );
      OggVorbis_File oggStream;
      vorbis_info*   vorbisInfo;
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

      int section;
      int bytesRead = 0;
      int result;
      do {
        result = int( ov_read( &oggStream, &vorbisBuffer[bytesRead], VORBIS_BUFFER_SIZE - bytesRead,
                               0, 2, 1, &section ) );
        bytesRead += result;
        if( result < 0 ) {
          ov_clear( &oggStream );
          log.printEnd( " Failed to decode Vorbis stream, error %d", result );
          return AL_NONE;
        }
      }
      while( result > 0 && bytesRead < VORBIS_BUFFER_SIZE );

      if( bytesRead == VORBIS_BUFFER_SIZE ) {
        ov_clear( &oggStream );
        log.printEnd( " Temporary buffer (%d bytes) too small to load sample", VORBIS_BUFFER_SIZE );
        return AL_NONE;
      }

      alGenBuffers( 1, &resource.id );
      alBufferData( resource.id, format, vorbisBuffer, bytesRead, int( vorbisInfo->rate ) );
      ov_clear( &oggStream );

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
    return resource.id;
  }

  void Context::releaseSound( int id )
  {
    Resource<uint>& resource = sounds[id];

    assert( uint( id ) < uint( translator.sounds.length() ) );
    assert( resource.nUsers > 0 );

    --resource.nUsers;
  }

  void Context::freeSound( int id )
  {
    Resource<uint>& resource = sounds[id];

    assert( uint( id ) < uint( translator.sounds.length() ) );
    assert( resource.nUsers == -2 );

    log.print( "Unloading sound '%s' ...", translator.sounds[id].name.cstr() );
    alDeleteBuffers( 1, &resource.id );
    resource.nUsers = -1;

    assert( alGetError() == AL_NO_ERROR );
    log.printEnd( " OK" );
  }

  BSP* Context::loadBSP( int id )
  {
    Resource<BSP*>& resource = bsps[id];

    assert( resource.object == null && resource.isUpdated == false );

    resource.object = new BSP( id );
    return resource.object;
  }

  void Context::releaseBSP( int id )
  {
    Resource<BSP*>& resource = bsps[id];

    assert( resource.object != null );

    delete resource.object;
    resource.object = null;
    resource.isUpdated = 0;
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
    for( int i = 0; i < lists.length(); ++i ) {
      if( lists[i].base == listId ) {
        glDeleteLists( lists[i].base, lists[i].count );
        lists.remove( i );
        break;
      }
    }
  }

  OBJ* Context::loadOBJ( const char* path )
  {
    Resource<OBJ*>* resource = objs.find( path );
    if( resource == null ) {
      resource = objs.add( path, Resource<OBJ*>() );
      resource->object = new OBJ( path );
    }
    ++resource->nUsers;
    return resource->object;
  }

  void Context::releaseOBJ( const char* path )
  {
    Resource<OBJ*>* resource = objs.find( path );

    assert( resource != null && resource->nUsers > 0 );

    if( resource != null ) {
      --resource->nUsers;

      if( resource->nUsers == 0 ) {
        delete resource->object;
        objs.exclude( path );
      }
    }
  }

  uint Context::loadStaticMD2( const char* path )
  {
    Resource<MD2*>* resource = staticMd2s.find( path );
    if( resource == null ) {
      resource = staticMd2s.add( path, Resource<MD2*>() );
      resource->object = new MD2( path );
      resource->object->genList();
    }
    ++resource->nUsers;
    return resource->object->list;
  }

  void Context::releaseStaticMD2( const char* path )
  {
    Resource<MD2*>* resource = staticMd2s.find( path );

    assert( resource != null && resource->nUsers > 0 );

    if( resource != null ) {
      --resource->nUsers;

      if( resource->nUsers == 0 ) {
        glDeleteLists( resource->object->list, 1 );
        delete resource->object;
        staticMd2s.exclude( path );
      }
    }
  }

  MD2* Context::loadMD2( const char* path )
  {
    Resource<MD2*>* resource = md2s.find( path );
    if( resource == null ) {
      resource = md2s.add( path, Resource<MD2*>() );
      resource->object = new MD2( path );
    }
    ++resource->nUsers;
    return resource->object;
  }

  void Context::releaseMD2( const char* path )
  {
    Resource<MD2*>* resource = md2s.find( path );

    assert( resource != null && resource->nUsers > 0 );

    if( resource != null ) {
      --resource->nUsers;

      if( resource->nUsers == 0 ) {
        delete resource->object;
        md2s.exclude( path );
      }
    }
  }

  uint Context::loadStaticMD3( const char* path )
  {
    Resource<MD3*>* resource = staticMd3s.find( path );
    if( resource == null ) {
      resource = staticMd3s.add( path, Resource<MD3*>() );
      resource->object = new MD3( path );
      resource->object->genList();
      resource->object->trim();
    }
    ++resource->nUsers;
    return resource->object->list;
  }

  void Context::releaseStaticMD3( const char* path )
  {
    Resource<MD3*>* resource = staticMd3s.find( path );

    assert( resource != null && resource->nUsers > 0 );

    if( resource != null ) {
      --resource->nUsers;

      if( resource->nUsers == 0 ) {
        glDeleteLists( resource->object->list, 1 );
        delete resource->object;
        staticMd3s.exclude( path );
      }
    }
  }

  MD3* Context::loadMD3( const char* path )
  {
    Resource<MD3*>* resource = md3s.find( path );
    if( resource == null ) {
      resource = md3s.add( path, Resource<MD3*>() );
      resource->object = new MD3( path );
    }
    ++resource->nUsers;
    return resource->object;
  }

  void Context::releaseMD3( const char* path )
  {
    Resource<MD3*>* resource = md3s.find( path );

    assert( resource != null && resource->nUsers > 0 );

    if( resource != null ) {
      --resource->nUsers;

      if( resource->nUsers == 0 ) {
        delete resource->object;
        md3s.exclude( path );
      }
    }
  }

  int Context::drawBSP( const Struct* str )
  {
    Resource<BSP*>& resource = bsps[str->bsp];

    if( resource.object == null ) {
      loadBSP( str->bsp );
    }
    resource.isUpdated = true;
    return resource.object->fullDraw( str );
  }

  void Context::drawBSPWater( const Struct* str )
  {
    Resource<BSP*>& resource = bsps[str->bsp];

    if( resource.object == null ) {
      loadBSP( str->bsp );
    }
    resource.isUpdated = true;
    return resource.object->fullDrawWater( str );
  }

  void Context::drawModel( const Object* obj, const Model* parent )
  {
    Model* const* value = models.find( obj->index );

    if( value == null ) {
      if( obj->flags & Object::MODEL_BIT ) {
        assert( !obj->clazz->modelType.isEmpty() );

        const Model::CreateFunc* createFunc = modelClasses.find( obj->clazz->modelType );
        if( createFunc == null ) {
          throw Exception( "Invalid Model" );
        }

        value = models.add( obj->index, ( *createFunc )( obj ) );
      }
      else {
        assert( obj->clazz->modelType.isEmpty() );

        value = models.add( obj->index, null );
      }
    }

    Model* model = *value;

    model->flags |= Model::UPDATED_BIT;
    model->draw( parent );
  }

  void Context::playAudio( const Object* obj, const Audio* parent )
  {
    Audio* const* value = audios.find( obj->index );

    if( value == null ) {
      if( obj->flags & Object::AUDIO_BIT ) {
        assert( !obj->clazz->audioType.isEmpty() );

        const Audio::CreateFunc* createFunc = audioClasses.find( obj->clazz->audioType );
        if( createFunc == null ) {
          throw Exception( "Invalid Audio" );
        }

        value = audios.add( obj->index, ( *createFunc )( obj ) );
      }
      else {
        assert( obj->clazz->audioType.isEmpty() );

        value = audios.add( obj->index, null );
      }
    }

    Audio* audio = *value;

    audio->flags |= Audio::UPDATED_BIT;
    audio->play( parent );
  }

  void Context::printLoad()
  {
    log.println( "Context load {" );
    log.indent();
    log.println( "Models: %.2f (%d)", models.loadFactor(), models.length() );
    log.println( "Audios: %.2f (%d)", audios.loadFactor(), audios.length() );
    log.unindent();
    log.println( "}" );
  }

  void Context::load()
  {
    log.print( "Loading Context ..." );

    for( int i = 0; i < translator.textures.length(); ++i ) {
      textures[i].nUsers = 0;
    }
    for( int i = 0; i < translator.sounds.length(); ++i ) {
      sounds[i].nUsers = -1;
    }
    for( int i = 0; i < translator.bsps.length(); ++i ) {
      bsps[i].object = null;
      bsps[i].isUpdated = false;
    }

    log.printEnd( " OK" );
  }

  void Context::unload()
  {
    log.println( "Unloading Context {" );
    log.indent();

    models.free();
    models.dealloc();
    audios.free();
    audios.dealloc();

    assert( alGetError() == AL_NO_ERROR );

    assert( lists.length() == 0 );

    foreach( i, objs.citer() ) {
      assert( i->nUsers == 0 );
    }
    foreach( i, staticMd2s.citer() ) {
      assert( i->nUsers == 0 );
    }
    foreach( i, md2s.citer() ) {
      assert( i->nUsers == 0 );
    }
    foreach( i, staticMd3s.citer() ) {
      assert( i->nUsers == 0 );
    }
    foreach( i, md3s.citer() ) {
      assert( i->nUsers == 0 );
    }

    lists.clear();
    lists.dealloc();
    objs.dealloc();
    staticMd2s.dealloc();
    md2s.dealloc();
    staticMd3s.dealloc();
    md3s.dealloc();

    for( int i = 0; i < translator.bsps.length(); ++i ) {
      delete bsps[i].object;
      bsps[i].object = null;
      bsps[i].nUsers = 0;
    }

    foreach( src, sources.citer() ) {
      alSourceStop( src->source );
      alDeleteSources( 1, &src->source );
      assert( alGetError() == AL_NO_ERROR );
    }
    foreach( src, contSources.citer() ) {
      alSourceStop( src->source );
      alDeleteSources( 1, &src->source );
      assert( alGetError() == AL_NO_ERROR );
    }

    sources.free();
    contSources.clear();
    contSources.dealloc();

    for( int i = 0; i < translator.textures.length(); ++i ) {
      assert( textures[i].nUsers == 0 );
    }
    for( int i = 0; i < translator.sounds.length(); ++i ) {
      assert( sounds[i].nUsers <= 0 );

      if( sounds[i].nUsers != -1 ) {
        alDeleteBuffers( 1, &sounds[i].id );
        assert( alGetError() == AL_NO_ERROR );
      }
    }

    assert( glGetError() == AL_NO_ERROR );
    assert( alGetError() == AL_NO_ERROR );

    Source::pool.free();

    OBJModel::pool.free();
    OBJVehicleModel::pool.free();
    MD2StaticModel::pool.free();
    MD2Model::pool.free();
    MD2WeaponModel::pool.free();
    MD3StaticModel::pool.free();
    ExplosionModel::pool.free();

    BasicAudio::pool.free();
    BotAudio::pool.free();

    log.unindent();
    log.println( "}" );
  }

  void Context::init()
  {
    log.print( "Initialising Context ..." );

    OZ_REGISTER_MODELCLASS( OBJ );
    OZ_REGISTER_MODELCLASS( Explosion );
    OZ_REGISTER_MODELCLASS( MD2Static );
    OZ_REGISTER_MODELCLASS( MD2 );
    OZ_REGISTER_MODELCLASS( MD2Weapon );
    OZ_REGISTER_MODELCLASS( MD3Static );
    OZ_REGISTER_MODELCLASS( OBJVehicle );

    OZ_REGISTER_AUDIOCLASS( Basic );
    OZ_REGISTER_AUDIOCLASS( Bot );

    textures = new Resource<uint>[translator.textures.length()];
    sounds   = new Resource<uint>[translator.sounds.length()];
    bsps     = new Resource<BSP*>[translator.bsps.length()];

    log.printEnd( " OK" );
  }

  void Context::free()
  {
    log.print( "Freeing Context ..." );

    delete[] textures;
    delete[] sounds;
    delete[] bsps;

    modelClasses.clear();
    modelClasses.dealloc();
    audioClasses.clear();
    audioClasses.dealloc();

    log.printEnd( " OK" );
  }

}
}
