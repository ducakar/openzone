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

#include "client/SMMModel.hpp"
#include "client/SMMVehicleModel.hpp"
#include "client/ExplosionModel.hpp"
#include "client/MD2Model.hpp"
#include "client/MD2WeaponModel.hpp"

#include "client/BasicAudio.hpp"
#include "client/BotAudio.hpp"
#include "client/VehicleAudio.hpp"

#ifdef OZ_BUILD_TOOLS
# include <SDL_image.h>
#endif
#include <GL/gl.h>
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
  Buffer Context::buffer;

#ifdef OZ_BUILD_TOOLS
  uint Context::buildTexture( const void* data, int width, int height, int bytesPerPixel,
                              bool wrap, int magFilter, int minFilter )
  {
    hard_assert( glGetError() == GL_NO_ERROR );
    hard_assert( bytesPerPixel == 3 || bytesPerPixel == 4 );

    GLenum sourceFormat = bytesPerPixel == 4 ? GL_RGBA : GL_RGB;
    GLenum internalFormat = bytesPerPixel == 4 ?
        GL_COMPRESSED_RGBA_S3TC_DXT5_EXT :
        GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;

    uint texId;
    glGenTextures( 1, &texId );
    glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
    glBindTexture( GL_TEXTURE_2D, texId );

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter );

    if( !wrap ) {
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
    }

    glTexImage2D( GL_TEXTURE_2D, 0, internalFormat, width, height, 0,
                  sourceFormat, GL_UNSIGNED_BYTE, data );
    glGenerateMipmap( GL_TEXTURE_2D );

    if( glGetError() != GL_NO_ERROR ) {
      glDeleteTextures( 1, &texId );
      texId = 0;

      hard_assert( glGetError() == GL_NO_ERROR );
    }

    hard_assert( glIsTexture( texId ) );

    return texId;
  }
#endif

  void Context::deleteSound( int id )
  {
    Resource<uint>& resource = sounds[id];

    hard_assert( uint( id ) < uint( translator.sounds.length() ) );
    hard_assert( resource.nUsers == -2 );

    log.print( "Unloading sound '%s' ...", translator.sounds[id].name.cstr() );
    alDeleteBuffers( 1, &resource.id );
    resource.nUsers = -1;

    hard_assert( alGetError() == AL_NO_ERROR );
    log.printEnd( " OK" );
  }

#ifdef OZ_BUILD_TOOLS
  uint Context::createTexture( const void* data, int width, int height, int bytesPerPixel,
                               bool wrap, int magFilter, int minFilter )
  {
    uint texNum = buildTexture( data, width, height, bytesPerPixel, wrap, magFilter, minFilter );

    if( texNum == ~0u ) {
      log.println( "Error creating texture from buffer" );
    }
    return texNum;
  }

  uint Context::loadRawTexture( const char* path, int* nMipmaps, bool wrap,
                                int magFilter, int minFilter )
  {
    log.print( "Loading raw texture '%s' ...", path );

    SDL_Surface* image = IMG_Load( path );
    if( image == null ) {
      log.printEnd( " No such file" );
      return 0;
    }
    if( image->format->BitsPerPixel != 24 && image->format->BitsPerPixel != 32 ) {
      log.printEnd( " Wrong format. Should be 24 bpp RGB or 32 bpp RGBA" );
      return 0;
    }
    log.printEnd( " OK, %s", image->format->BitsPerPixel == 24 ? "RGB" : "RGBA" );

    int bytesPerPixel = image->format->BitsPerPixel / 8;
    int texNum = createTexture( image->pixels, image->w, image->h, bytesPerPixel, wrap,
                                magFilter, minFilter );

    SDL_FreeSurface( image );

    hard_assert( glIsTexture( texNum ) );

    if( nMipmaps != null ) {
      *nMipmaps = 0;
      for( int i = 0; i < 1000; ++i ) {
        int mipmapSize;
        glGetTexLevelParameteriv( GL_TEXTURE_2D, i, GL_TEXTURE_COMPRESSED_IMAGE_SIZE, &mipmapSize );

        if( glGetError() != GL_NO_ERROR ) {
          break;
        }

        ++*nMipmaps;
      }
    }

    return texNum;
  }

  void Context::writeTexture( uint id, int nMipmaps, OutputStream* stream )
  {
    glBindTexture( GL_TEXTURE_2D, id );

    int internalFormat, magFilter, minFilter, wrap;
    glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &internalFormat );
    glGetTexParameteriv( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, &magFilter );
    glGetTexParameteriv( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, &minFilter );
    glGetTexParameteriv( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, &wrap );

    stream->writeInt( nMipmaps );
    stream->writeInt( internalFormat );
    stream->writeInt( magFilter );
    stream->writeInt( minFilter );
    stream->writeInt( wrap );

    for( int i = 0; i < nMipmaps; ++i ) {
      int width, height, size;

      glGetTexLevelParameteriv( GL_TEXTURE_2D, i, GL_TEXTURE_COMPRESSED_IMAGE_SIZE, &size );
      glGetTexLevelParameteriv( GL_TEXTURE_2D, i, GL_TEXTURE_WIDTH, &width );
      glGetTexLevelParameteriv( GL_TEXTURE_2D, i, GL_TEXTURE_HEIGHT, &height );

      stream->writeInt( width );
      stream->writeInt( height );
      stream->writeInt( size );
      glGetCompressedTexImage( GL_TEXTURE_2D, i, stream->prepareWrite( size ) );
    }

    hard_assert( glGetError() == GL_NO_ERROR );
  }
#endif

  uint Context::loadTexture( const char* path )
  {
    log.print( "Loading texture '%s' ...", path );

    if( !buffer.read( path ) ) {
      log.printEnd( " No such file" );
      return 0;
    }

    InputStream is = buffer.inputStream();
    uint id = readTexture( &is );

    if( id == 0 ) {
      log.printEnd( " Failed" );
      return 0;
    }

    log.printEnd( " OK" );
    return id;
  }

  uint Context::readTexture( InputStream* stream )
  {
    hard_assert( glGetError() == GL_NO_ERROR );

    uint texId;
    glGenTextures( 1, &texId );
    glBindTexture( GL_TEXTURE_2D, texId );

    int nMipmaps       = stream->readInt();
    int internalFormat = stream->readInt();
    int magFilter      = stream->readInt();
    int minFilter      = stream->readInt();
    int wrap           = stream->readInt();

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter );

    if( !wrap ) {
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
    }

    for( int i = 0; i < nMipmaps; ++i ) {
      int width = stream->readInt();
      int height = stream->readInt();
      int size = stream->readInt();

      glCompressedTexImage2D( GL_TEXTURE_2D, i, internalFormat, width, height, 0, size,
                              stream->prepareRead( size ) );
    }

    if( glGetError() != GL_NO_ERROR ) {
      glDeleteTextures( 1, &texId );
      texId = 0;

      hard_assert( glGetError() == GL_NO_ERROR );
    }

    hard_assert( glGetError() == GL_NO_ERROR );

    return texId;
  }

  uint Context::requestTexture( int id )
  {
    Resource<uint>& resource = textures[id];

    if( resource.nUsers > 0 ) {
      ++resource.nUsers;
      return resource.id;
    }
    resource.nUsers = 1;

    const String& name = translator.textures[id].name;

    log.print( "Loading texture '%s' ...", name.cstr() );

    resource.id = GL_NONE;

    if( buffer.read( "bsp/tex/" + name + ".ozcTex" ) ) {
      InputStream is = buffer.inputStream();

      resource.id = readTexture( &is );
    }

    if( resource.id == 0 ) {
      log.printEnd( " Failed" );
      return 0;
    }

    log.printEnd( " OK" );
    return resource.id;
  }

  void Context::releaseTexture( int id )
  {
    Resource<uint>& resource = textures[id];

    hard_assert( uint( id ) < uint( translator.textures.length() ) );
    hard_assert( resource.nUsers > 0 );

    --resource.nUsers;

    if( resource.nUsers == 0 ) {
      log.print( "Unloading texture '%s' ...", translator.textures[id].name.cstr() );
      glDeleteTextures( 1, &resource.id );
      hard_assert( glGetError() == GL_NO_ERROR );
      log.printEnd( " OK" );
    }
  }

  uint Context::requestSound( int id )
  {
    Resource<uint>& resource = sounds[id];

    if( resource.nUsers != -1 ) {
      resource.nUsers = resource.nUsers < 0 ? 1 : resource.nUsers + 1;
      return resource.id;
    }

    resource.nUsers = 1;

    hard_assert( alGetError() == AL_NO_ERROR );

    log.print( "Loading sound '%s' ...", translator.sounds[id].name.cstr() );

    resource.id = alutCreateBufferFromFile( translator.sounds[id].path );

    if( resource.id == 0 ) {
      log.printEnd( " Failed" );
      return 0;
    }

    log.printEnd( " OK" );
    return resource.id;
  }

  void Context::releaseSound( int id )
  {
    Resource<uint>& resource = sounds[id];

    hard_assert( uint( id ) < uint( translator.sounds.length() ) );
    hard_assert( resource.nUsers > 0 );

    --resource.nUsers;
  }

  SMM* Context::requestSMM( int id )
  {
    Resource<SMM*>* resource = smms.find( id );

    if( resource == null ) {
      resource = smms.add( id, Resource<SMM*>() );
      resource->object = new SMM( id );
      resource->nUsers = 0;
    }

    ++resource->nUsers;
    return resource->object;
  }

  void Context::releaseSMM( int id )
  {
    Resource<SMM*>* resource = smms.find( id );

    hard_assert( resource != null && resource->nUsers > 0 );

    --resource->nUsers;
  }

  MD2* Context::requestMD2( int id )
  {
    Resource<MD2*>* resource = md2s.find( id );

    if( resource == null ) {
      resource = md2s.add( id, Resource<MD2*>() );
      resource->object = new MD2( id );
      resource->nUsers = 0;
    }

    ++resource->nUsers;
    return resource->object;
  }

  void Context::releaseMD2( int id )
  {
    Resource<MD2*>* resource = md2s.find( id );

    hard_assert( resource != null && resource->nUsers > 0 );

    --resource->nUsers;
  }

//   MD3* Context::loadMD3( const char* path )
//   {
//     Resource<MD3*>* resource = md3s.find( path );
//
//     if( resource == null ) {
//       resource = md3s.add( path, Resource<MD3*>() );
//       resource->object = new MD3( path );
//       resource->nUsers = 0;
//     }
//
//     ++resource->nUsers;
//     return resource->object;
//   }
//
//   void Context::releaseMD3( const char* path )
//   {
//     Resource<MD3*>* resource = md3s.find( path );
//
//     hard_assert( resource != null && resource->nUsers > 0 );
//
//     --resource->nUsers;
//   }

  void Context::drawBSP( const Struct* str, int mask )
  {
    Resource<BSP*>& resource = bsps[str->bsp];

    if( resource.object == null ) {
      resource.object = new BSP( str->bsp );
      resource.nUsers = 1;
    }
    else if( resource.object->isLoaded ) {
      // we don't count users, just to show there is at least one
      resource.nUsers = 1;
      resource.object->draw( str, mask );
    }
  }

  void Context::playBSP( const Struct* str )
  {
    Resource<BSP*>& resource = bsps[str->bsp];

    if( resource.object == null ) {
      resource.object = new BSP( str->bsp );
      resource.nUsers = 1;
    }
    else if( resource.object->isLoaded ) {
      // we don't count users, just to show there is at least one
      resource.nUsers = 1;
      resource.object->play( str );
    }
  }

  void Context::drawModel( const Object* obj, const Model* parent )
  {
    hard_assert( obj->flags & Object::MODEL_BIT );

    Model* const* value = models.find( obj->index );

    if( value == null ) {
      hard_assert( !obj->clazz->modelType.isEmpty() );

      const Model::CreateFunc* createFunc = modelClasses.find( obj->clazz->modelType );
      if( createFunc == null ) {
        throw Exception( "Invalid Model '" + obj->clazz->modelType + "'" );
      }

      value = models.add( obj->index, ( *createFunc )( obj ) );
    }

    Model* model = *value;

    model->flags |= Model::UPDATED_BIT;
    model->draw( parent );
  }

  void Context::playAudio( const Object* obj, const Audio* parent )
  {
    hard_assert( obj->flags & Object::AUDIO_BIT );

    Audio* const* value = audios.find( obj->index );

    if( value == null ) {
      hard_assert( !obj->clazz->audioType.isEmpty() );

      const Audio::CreateFunc* createFunc = audioClasses.find( obj->clazz->audioType );
      if( createFunc == null ) {
        throw Exception( "Invalid Audio '" + obj->clazz->audioType + "'" );
      }

      value = audios.add( obj->index, ( *createFunc )( obj ) );
    }

    Audio* audio = *value;

    audio->flags |= Audio::UPDATED_BIT;
    audio->play( parent );
  }

  void Context::updateLoad()
  {
    maxModels     = max( maxModels, models.length() );
    maxAudios     = max( maxAudios, audios.length() );
    maxSources    = max( maxSources, sources.length() );
    maxBSPSources = max( maxBSPSources, bspSources.length() );
    maxObjSources = max( maxObjSources, objSources.length() );
  }

  void Context::printLoad()
  {
    log.println( "Context maximum load {" );
    log.indent();
    log.println( "Models       %d (hashtable load %.2f)", maxModels,
                 float( maxModels ) / float( models.capacity() ) );
    log.println( "Audios       %d (hashtable load %.2f)", maxAudios,
                 float( maxAudios ) / float( audios.capacity() ) );
    log.println( "Sources      %d", maxSources );
    log.println( "BSPSources   %d (hashtable load %.2f)", maxBSPSources,
                 float( maxBSPSources ) / float( bspSources.capacity() ) );
    log.println( "ObjSources   %d (hashtable load %.2f)", maxObjSources,
                 float( maxObjSources ) / float( bspSources.capacity() ) );
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
      bsps[i].nUsers = 0;
    }

    maxModels     = 0;
    maxAudios     = 0;
    maxSources    = 0;
    maxBSPSources = 0;
    maxObjSources = 0;

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

    hard_assert( alGetError() == AL_NO_ERROR );

    for( auto i = smms.citer(); i.isValid(); ) {
      auto resource = i;
      ++i;

      hard_assert( resource->nUsers == 0 );

      delete resource->object;
      smms.exclude( resource.key() );
    }
    for( auto i = md2s.citer(); i.isValid(); ) {
      auto resource = i;
      ++i;

      hard_assert( resource->nUsers == 0 );

      delete resource->object;
      md2s.exclude( resource.key() );
    }
    for( auto i = md3s.citer(); i.isValid(); ) {
      auto resource = i;
      ++i;

      hard_assert( resource->nUsers == 0 );

      delete resource->object;
      md3s.exclude( resource.key() );
    }
    for( int i = 0; i < translator.bsps.length(); ++i ) {
      delete bsps[i].object;
      bsps[i].object = null;
      bsps[i].nUsers = 0;
    }

    smms.dealloc();
    md2s.dealloc();
    md3s.dealloc();

    foreach( src, sources.citer() ) {
      alDeleteSources( 1, &src->source );
      hard_assert( alGetError() == AL_NO_ERROR );
    }
    foreach( src, bspSources.citer() ) {
      alDeleteSources( 1, &src->source );
      hard_assert( alGetError() == AL_NO_ERROR );
    }
    foreach( src, objSources.citer() ) {
      alDeleteSources( 1, &src->source );
      hard_assert( alGetError() == AL_NO_ERROR );
    }

    sources.free();
    bspSources.clear();
    bspSources.dealloc();
    objSources.clear();
    objSources.dealloc();

    for( int i = 0; i < translator.textures.length(); ++i ) {
      hard_assert( textures[i].nUsers == 0 );
    }
    for( int i = 0; i < translator.sounds.length(); ++i ) {
      hard_assert( sounds[i].nUsers <= 0 );

      if( sounds[i].nUsers != -1 ) {
        sounds[i].nUsers = -2;
        deleteSound( i );
      }
    }

    hard_assert( glGetError() == AL_NO_ERROR );
    hard_assert( alGetError() == AL_NO_ERROR );

    Source::pool.free();

    SMMModel::pool.free();
    SMMVehicleModel::pool.free();
    ExplosionModel::pool.free();
    MD2Model::pool.free();
    MD2WeaponModel::pool.free();

    BasicAudio::pool.free();
    BotAudio::pool.free();
    VehicleAudio::pool.free();

    log.unindent();
    log.println( "}" );
  }

  void Context::init()
  {
    textures = null;
    sounds   = null;
    bsps     = null;

    log.print( "Initialising Context ..." );

    OZ_REGISTER_MODELCLASS( SMM );
    OZ_REGISTER_MODELCLASS( SMMVehicle );
    OZ_REGISTER_MODELCLASS( Explosion );
    OZ_REGISTER_MODELCLASS( MD2 );
    OZ_REGISTER_MODELCLASS( MD2Weapon );

    OZ_REGISTER_AUDIOCLASS( Basic );
    OZ_REGISTER_AUDIOCLASS( Bot );
    OZ_REGISTER_AUDIOCLASS( Vehicle );

    if( translator.textures.length() == 0 ) {
      throw Exception( "Context: textures missing!" );
    }
    if( translator.sounds.length() == 0 ) {
      throw Exception( "Context: sounds missing!" );
    }
    if( translator.bsps.length() == 0 ) {
      throw Exception( "Context: BSPs missing!" );
    }

    textures = new Resource<uint>[translator.textures.length()];
    sounds   = new Resource<uint>[translator.sounds.length()];
    bsps     = new Resource<BSP*>[translator.bsps.length()];

    buffer.alloc( BUFFER_SIZE );

    log.printEnd( " OK" );
  }

  void Context::free()
  {
    log.print( "Freeing Context ..." );

    buffer.dealloc();

    delete[] textures;
    delete[] sounds;
    delete[] bsps;

    textures = null;
    sounds   = null;
    bsps     = null;

    modelClasses.clear();
    modelClasses.dealloc();
    audioClasses.clear();
    audioClasses.dealloc();

    log.printEnd( " OK" );
  }

}
}
