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

#include "client/OpenGL.hpp"
#include "client/OpenAL.hpp"

#ifdef OZ_SDK
# include <SDL_image.h>
#endif

#define OZ_REGISTER_MODELCLASS( name ) \
  modelClasses.add( #name, &name##Model::create )

#define OZ_REGISTER_AUDIOCLASS( name ) \
  audioClasses.add( #name, &name##Audio::create )

namespace oz
{
namespace client
{

  Context context;

  const int Context::DEFAULT_MAG_FILTER = GL_LINEAR;
  const int Context::DEFAULT_MIN_FILTER = GL_LINEAR_MIPMAP_LINEAR;

  Pool<Context::Source> Context::Source::pool;
  Buffer Context::buffer;

#ifdef OZ_SDK
  uint Context::buildTexture( const void* data, int width, int height, int bytesPerPixel,
                              bool wrap, int magFilter, int minFilter )
  {
    OZ_GL_CHECK_ERROR();
    hard_assert( bytesPerPixel == 3 || bytesPerPixel == 4 );

    uint sourceFormat = bytesPerPixel == 4 ? GL_RGBA : GL_RGB;
#ifdef OZ_GL_S3TC
    int internalFormat = bytesPerPixel == 4 ?
        GL_COMPRESSED_RGBA_S3TC_DXT5_EXT :
        GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
#else
    int internalFormat = int( sourceFormat );
#endif

    bool doGenerateMipmaps = false;

    uint texId;
    glGenTextures( 1, &texId );
    glBindTexture( GL_TEXTURE_2D, texId );

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter );

    switch( magFilter ) {
      case GL_NEAREST:
      case GL_LINEAR: {
        break;
      }
      default: {
        throw Exception( "Invalid texture magnification filter" );
      }
    }

    switch( minFilter ) {
      case GL_NEAREST:
      case GL_LINEAR: {
        break;
      }
      case GL_NEAREST_MIPMAP_NEAREST:
      case GL_NEAREST_MIPMAP_LINEAR:
      case GL_LINEAR_MIPMAP_NEAREST:
      case GL_LINEAR_MIPMAP_LINEAR: {
        doGenerateMipmaps = true;
        break;
      }
      default: {
        throw Exception( "Invalid texture minification filter" );
      }
    }

    if( !wrap ) {
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
    }

    glTexImage2D( GL_TEXTURE_2D, 0, internalFormat, width, height, 0,
                  sourceFormat, GL_UNSIGNED_BYTE, data );

    if( doGenerateMipmaps ) {
      glGenerateMipmap( GL_TEXTURE_2D );
    }

    glBindTexture( GL_TEXTURE_2D, 0 );

    if( glGetError() != GL_NO_ERROR || !glIsTexture( texId ) ) {
      glDeleteTextures( 1, &texId );
      texId = 0;

      OZ_GL_CHECK_ERROR();
    }

    return texId;
  }
#endif

  void Context::addSource( uint srcId, int sample )
  {
    hard_assert( sounds[sample].nUsers > 0 );

    ++sounds[sample].nUsers;
    sources.add( new Source( srcId, sample ) );
  }

  void Context::addBSPSource( uint srcId, int sample, int key )
  {
    hard_assert( sounds[sample].nUsers > 0 );

    ++sounds[sample].nUsers;
    bspSources.add( key, ContSource( srcId, sample ) );
  }

  void Context::addObjSource( uint srcId, int sample, int key )
  {
    hard_assert( sounds[sample].nUsers > 0 );

    ++sounds[sample].nUsers;
    objSources.add( key, ContSource( srcId, sample ) );
  }

  void Context::removeSource( Source* source, Source* prev )
  {
    int sample = source->sample;

    hard_assert( sounds[sample].nUsers > 0 );

    sources.remove( source, prev );
    delete source;
    releaseSound( sample );
  }

  void Context::removeBSPSource( ContSource* contSource, int key )
  {
    int sample = contSource->sample;

    hard_assert( sounds[sample].nUsers > 0 );

    bspSources.exclude( key );
    releaseSound( sample );
  }

  void Context::removeObjSource( ContSource* contSource, int key )
  {
    int sample = contSource->sample;

    hard_assert( sounds[sample].nUsers > 0 );

    objSources.exclude( key );
    releaseSound( sample );
  }

  Context::Context() : textures( null ), sounds( null ), bsps( null )
  {}

#ifdef OZ_SDK
  uint Context::createTexture( const void* data, int width, int height, int bytesPerPixel,
                               bool wrap, int magFilter, int minFilter )
  {
    uint texId = buildTexture( data, width, height, bytesPerPixel, wrap, magFilter, minFilter );

    if( texId == 0 ) {
      log.println( "Error while creating texture from buffer" );
      throw Exception( "Texture loading failed" );
    }
    return texId;
  }

  uint Context::loadRawTexture( const char* path, bool wrap, int magFilter, int minFilter )
  {
    log.print( "Loading raw texture '%s' ...", path );

    SDL_Surface* image = IMG_Load( path );
    if( image == null ) {
      log.printEnd( " No such file" );
      throw Exception( "Texture loading failed" );
    }
    if( image->format->BitsPerPixel != 24 && image->format->BitsPerPixel != 32 ) {
      log.printEnd( " Wrong format. Should be 24 bpp RGB or 32 bpp RGBA" );
      throw Exception( "Texture loading failed" );
    }
    log.printEnd( " %s ... OK", image->format->BitsPerPixel == 24 ? "RGB" : "RGBA" );

    int  bytesPerPixel = image->format->BitsPerPixel / 8;
    uint texId = createTexture( image->pixels, image->w, image->h, bytesPerPixel, wrap,
                                 magFilter, minFilter );

    SDL_FreeSurface( image );

    if( texId == 0 || !glIsTexture( texId ) ) {
      throw Exception( "Texture loading failed" );
    }

    return texId;
  }

  void Context::writeTexture( uint id, OutputStream* stream )
  {
    glBindTexture( GL_TEXTURE_2D, id );

    int wrap, magFilter, minFilter, nMipmaps, internalFormat;

    glGetTexParameteriv( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, &wrap );
    glGetTexParameteriv( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, &magFilter );
    glGetTexParameteriv( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, &minFilter );
    glGetTexParameteriv( GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, &nMipmaps );

    int width;
    for( nMipmaps = 0; nMipmaps < 1000; ++nMipmaps ) {
      glGetTexLevelParameteriv( GL_TEXTURE_2D, nMipmaps, GL_TEXTURE_WIDTH, &width );

      if( width == 0 ) {
        break;
      }
    }

    glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &internalFormat );
    OZ_GL_CHECK_ERROR();

    stream->writeInt( wrap );
    stream->writeInt( magFilter );
    stream->writeInt( minFilter );
    stream->writeInt( nMipmaps );
    stream->writeInt( internalFormat );

    for( int i = 0; i < nMipmaps; ++i ) {
      int width, height, size;

      glGetTexLevelParameteriv( GL_TEXTURE_2D, i, GL_TEXTURE_WIDTH, &width );
      glGetTexLevelParameteriv( GL_TEXTURE_2D, i, GL_TEXTURE_HEIGHT, &height );
#ifdef OZ_GL_S3TC
      glGetTexLevelParameteriv( GL_TEXTURE_2D, i, GL_TEXTURE_COMPRESSED_IMAGE_SIZE, &size );
#else
      size = width * height * 4;
#endif

      stream->writeInt( width );
      stream->writeInt( height );
      stream->writeInt( size );

#ifdef OZ_GL_S3TC
      glGetCompressedTexImage( GL_TEXTURE_2D, i, stream->prepareWrite( size ) );
#else
      glGetTexImage( GL_TEXTURE_2D, i, GL_RGBA, GL_UNSIGNED_BYTE, stream->prepareWrite( size ) );
#endif
    }

    OZ_GL_CHECK_ERROR();
  }
#endif

  uint Context::loadTexture( const char* path )
  {
    log.print( "Loading texture '%s' ...", path );

    if( !buffer.read( path ) ) {
      log.printEnd( " No such file" );
      throw Exception( "Texture loading failed" );
    }

    InputStream is = buffer.inputStream();
    uint id = readTexture( &is );

    if( id == 0 ) {
      log.printEnd( " Failed" );
      throw Exception( "Texture loading failed" );
    }

    log.printEnd( " OK" );
    return id;
  }

  uint Context::readTexture( InputStream* stream )
  {
    OZ_GL_CHECK_ERROR();

    uint texId;
    glGenTextures( 1, &texId );
    glBindTexture( GL_TEXTURE_2D, texId );

    int wrap           = stream->readInt();
    int magFilter      = stream->readInt();
    int minFilter      = stream->readInt();
    int nMipmaps       = stream->readInt();
    int internalFormat = stream->readInt();

#ifdef OZ_GL_S3TC
    hard_assert( internalFormat == GL_COMPRESSED_RGB_S3TC_DXT1_EXT ||
                 internalFormat == GL_COMPRESSED_RGBA_S3TC_DXT5_EXT );
#else
    hard_assert( internalFormat == GL_RGB || internalFormat == GL_RGBA );
#endif

    if( !wrap ) {
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
    }

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter );

    for( int i = 0; i < nMipmaps; ++i ) {
      int width = stream->readInt();
      int height = stream->readInt();
      int size = stream->readInt();

#ifdef OZ_GL_S3TC
      glCompressedTexImage2D( GL_TEXTURE_2D, i, uint( internalFormat ), width, height, 0,
                              size, stream->prepareRead( size ) );
#else
      glTexImage2D( GL_TEXTURE_2D, i, internalFormat, width, height, 0,
                    GL_RGBA, GL_UNSIGNED_BYTE, stream->prepareRead( size ) );
#endif
    }

    if( glGetError() != GL_NO_ERROR || !glIsTexture( texId ) ) {
      glDeleteTextures( 1, &texId );
      texId = 0;
    }

    OZ_GL_CHECK_ERROR();

    return texId;
  }

  uint Context::requestTexture( int id )
  {
    Resource<uint>& resource = textures[id];

    if( resource.nUsers != 0 ) {
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
      throw Exception( "Texture loading failed" );
    }

    log.printEnd( " OK" );
    return resource.id;
  }

  void Context::releaseTexture( int id )
  {
    Resource<uint>& resource = textures[id];

    hard_assert( uint( id ) < uint( translator.textures.length() ) && resource.nUsers > 0 );

    --resource.nUsers;

    if( resource.nUsers == 0 ) {
      log.print( "Unloading texture '%s' ...", translator.textures[id].name.cstr() );
      glDeleteTextures( 1, &resource.id );

      OZ_GL_CHECK_ERROR();

      log.printEnd( " OK" );
    }
  }

  uint Context::requestSound( int id )
  {
    Resource<uint>& resource = sounds[id];

    if( resource.nUsers != 0 ) {
      ++resource.nUsers;
      return resource.id;
    }

    resource.nUsers = 1;

    OZ_AL_CHECK_ERROR();

    const String& name = translator.sounds[id].name;
    const String& path = translator.sounds[id].path;

    log.print( "Loading sound '%s' ...", name.cstr() );

    uint   length;
    ubyte* data;

    SDL_AudioSpec audioSpec;

    audioSpec.freq     = DEFAULT_AUDIO_FREQ;
    audioSpec.format   = DEFAULT_AUDIO_FORMAT;
    audioSpec.channels = 1;
    audioSpec.samples  = 0;

    if( SDL_LoadWAV( path, &audioSpec, &data, &length ) == null ) {
      throw Exception( "Failed to load sound" );
    }

    if( audioSpec.channels != 1 ||
        ( audioSpec.format != AUDIO_U8 && audioSpec.format != AUDIO_S16 ) )
    {
      log.printEnd( " Failed, format should be mono U8 mono or S16LE mono" );
        throw Exception( "Invalid sound format, should be U8 mono or S16LE mono" );
    }

    ALenum format = audioSpec.format == AUDIO_U8 ? AL_FORMAT_MONO8 : AL_FORMAT_MONO16;

    alGenBuffers( 1, &resource.id );
    alBufferData( resource.id, format, data, int( length ), audioSpec.freq );

    SDL_FreeWAV( data );

    OZ_AL_CHECK_ERROR();

    if( resource.id == 0 ) {
      log.printEnd( " Failed" );
      throw Exception( "Sound loading failed" );
    }

    log.printEnd( " %s %d Hz ... OK", format == AL_FORMAT_MONO8 ? "U8" : "S16LE", audioSpec.freq );
    return resource.id;
  }

  void Context::releaseSound( int id )
  {
    Resource<uint>& resource = sounds[id];

    hard_assert( uint( id ) < uint( translator.sounds.length() ) && resource.nUsers > 0 );

    --resource.nUsers;

    if( resource.nUsers == 0 ) {
      log.print( "Unloading sound '%s' ...", translator.sounds[id].name.cstr() );
      alDeleteBuffers( 1, &resource.id );

      OZ_AL_CHECK_ERROR();

      log.printEnd( " OK" );
    }
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

#ifndef NDEBUG
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
#endif

  void Context::load()
  {
    log.print( "Loading Context ..." );

    for( int i = 0; i < translator.textures.length(); ++i ) {
      textures[i].nUsers = 0;
    }
    for( int i = 0; i < translator.sounds.length(); ++i ) {
      sounds[i].nUsers = 0;
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

    OZ_AL_CHECK_ERROR();

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

    while( !sources.isEmpty() ) {
      alDeleteSources( 1, &sources.first()->id );
      removeSource( sources.first(), null );
      OZ_AL_CHECK_ERROR();
    }
    for( auto i = bspSources.iter(); i.isValid(); ) {
      auto src = i;
      ++i;

      alDeleteSources( 1, &src->id );
      removeBSPSource( src, src.key() );
      OZ_AL_CHECK_ERROR();
    }
    for( auto i = objSources.iter(); i.isValid(); ) {
      auto src = i;
      ++i;

      alDeleteSources( 1, &src->id );
      removeObjSource( src, src.key() );
      OZ_AL_CHECK_ERROR();
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
      hard_assert( sounds[i].nUsers == 0 );
    }

    hard_assert( glGetError() == AL_NO_ERROR );
    OZ_AL_CHECK_ERROR();

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
