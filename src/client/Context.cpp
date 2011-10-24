/*
 *  Context.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
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
#include "client/MD3Model.hpp"

#include "client/BasicAudio.hpp"
#include "client/BotAudio.hpp"
#include "client/VehicleAudio.hpp"

#include "client/OpenGL.hpp"
#include "client/OpenAL.hpp"

#ifdef OZ_TOOLS
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

#ifndef OZ_TOOLS

  Pool<Context::Source> Context::Source::pool;
  Buffer Context::buffer;
  bool Context::isS3TCSupported;

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

  uint Context::loadTexture( const char* path )
  {
    log.print( "Loading texture '%s' ...", path );

    if( !buffer.read( path ) ) {
      log.printEnd( " No such file" );
      throw Exception( "Texture loading failed" );
    }

    InputStream is = buffer.inputStream();
    uint id = readTexture( &is );

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

    bool usesS3TC = internalFormat == GL_COMPRESSED_RGB_S3TC_DXT1_EXT ||
        internalFormat == GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;

    if( !isS3TCSupported && usesS3TC ) {
      throw Exception( "Texture uses S3 texture compression but texture compression disabled" );
    }

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

      if( usesS3TC ) {
        glCompressedTexImage2D( GL_TEXTURE_2D, i, uint( internalFormat ), width, height, 0,
                                size, stream->prepareRead( size ) );
      }
      else {
        glTexImage2D( GL_TEXTURE_2D, i, internalFormat, width, height, 0,
                      GL_RGBA, GL_UNSIGNED_BYTE, stream->prepareRead( size ) );
      }
    }

    if( glGetError() != GL_NO_ERROR || !glIsTexture( texId ) ) {
      glDeleteTextures( 1, &texId );

      throw Exception( "Texture loading failed" );
    }

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

    const String& name = library.textures[id].name;

    log.print( "Loading texture '%s' ...", name.cstr() );

    resource.id = GL_NONE;

    if( buffer.read( "bsp/" + name + ".ozcTex" ) ) {
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

    hard_assert( uint( id ) < uint( library.textures.length() ) && resource.nUsers > 0 );

    --resource.nUsers;

    if( resource.nUsers == 0 ) {
      log.print( "Unloading texture '%s' ...", library.textures[id].name.cstr() );
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

    const String& name = library.sounds[id].name;
    const String& path = library.sounds[id].path;

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

    hard_assert( uint( id ) < uint( library.sounds.length() ) && resource.nUsers > 0 );

    --resource.nUsers;

    if( resource.nUsers == 0 ) {
      log.print( "Unloading sound '%s' ...", library.sounds[id].name.cstr() );
      alDeleteBuffers( 1, &resource.id );

      OZ_AL_CHECK_ERROR();

      log.printEnd( " OK" );
    }
  }

  SMM* Context::requestSMM( int id )
  {
    Resource<SMM*>& resource = smms[id];

    if( resource.object == null ) {
      resource.object = new SMM( id );

      hard_assert( resource.nUsers == 0 );
    }

    ++resource.nUsers;
    return resource.object;
  }

  void Context::releaseSMM( int id )
  {
    Resource<SMM*>& resource = smms[id];

    hard_assert( resource.object != null && resource.nUsers > 0 );

    --resource.nUsers;
  }

  MD2* Context::requestMD2( int id )
  {
    Resource<MD2*>& resource = md2s[id];

    if( resource.object == null ) {
      resource.object = new MD2( id );

      hard_assert( resource.nUsers == 0 );
    }

    ++resource.nUsers;
    return resource.object;
  }

  void Context::releaseMD2( int id )
  {
    Resource<MD2*>& resource = md2s[id];

    hard_assert( resource.object != null && resource.nUsers > 0 );

    --resource.nUsers;
  }

  MD3* Context::requestMD3( int id )
  {
    Resource<MD3*>& resource = md3s[id];

    if( resource.object == null ) {
      resource.object = new MD3( id );

      hard_assert( resource.nUsers == 0 );
    }

    ++resource.nUsers;
    return resource.object;
  }

  void Context::releaseMD3( int id )
  {
    Resource<MD3*>& resource = md3s[id];

    hard_assert( resource.object != null && resource.nUsers > 0 );

    --resource.nUsers;
  }

  void Context::drawBSP( const Struct* str, int mask )
  {
    Resource<BSP*>& resource = bsps[str->id];

    // we don't count users, just to show there is at least one
    resource.nUsers = 1;

    if( resource.object == null ) {
      resource.object = new BSP( str->id );
    }
    else if( resource.object->isLoaded ) {
      resource.object->draw( str, mask );
    }
  }

  void Context::playBSP( const Struct* str )
  {
    Resource<BSP*>& resource = bsps[str->id];

    // we don't count users, just to show there is at least one
    resource.nUsers = 1;

    if( resource.object == null ) {
      resource.object = new BSP( str->id );
    }
    else if( resource.object->isLoaded ) {
      resource.object->play( str );
    }
  }

  void Context::drawModel( const Object* obj, const Model* parent, int mask )
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
    model->draw( parent, mask );
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

# ifndef NDEBUG
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
# endif

  void Context::load()
  {}

  void Context::unload()
  {
    log.println( "Unloading Context {" );
    log.indent();

    models.free();
    models.dealloc();
    audios.free();
    audios.dealloc();

    OZ_AL_CHECK_ERROR();

    for( int i = 0; i < library.bsps.length(); ++i ) {
      delete bsps[i].object;
      bsps[i].object = null;
      bsps[i].nUsers = 0;
    }
    for( int i = 0; i < library.models.length(); ++i ) {
      delete smms[i].object;
      smms[i].object = null;
      smms[i].nUsers = 0;

      delete md2s[i].object;
      md2s[i].object = null;
      md2s[i].nUsers = 0;

      delete md3s[i].object;
      md3s[i].object = null;
      md3s[i].nUsers = 0;
    }

    while( !sources.isEmpty() ) {
      alDeleteSources( 1, &sources.first()->id );
      removeSource( sources.first(), null );
      OZ_AL_CHECK_ERROR();
    }
    for( auto i = bspSources.iter(); i.isValid(); ) {
      auto src = i;
      ++i;

      alDeleteSources( 1, &src.value().id );
      removeBSPSource( &src.value(), src.key() );
      OZ_AL_CHECK_ERROR();
    }
    for( auto i = objSources.iter(); i.isValid(); ) {
      auto src = i;
      ++i;

      alDeleteSources( 1, &src.value().id );
      removeObjSource( &src.value(), src.key() );
      OZ_AL_CHECK_ERROR();
    }

    sources.free();
    bspSources.clear();
    bspSources.dealloc();
    objSources.clear();
    objSources.dealloc();

    for( int i = 0; i < library.textures.length(); ++i ) {
      hard_assert( textures[i].nUsers == 0 );
    }
    for( int i = 0; i < library.sounds.length(); ++i ) {
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
    MD3Model::pool.free();

    BasicAudio::pool.free();
    BotAudio::pool.free();
    VehicleAudio::pool.free();

    log.unindent();
    log.println( "}" );
  }

  void Context::init()
  {
    log.print( "Initialising Context ..." );

    textures = null;
    sounds   = null;
    bsps     = null;
    smms     = null;
    md2s     = null;
    md3s     = null;

    isS3TCSupported = false;

    OZ_REGISTER_MODELCLASS( SMM );
    OZ_REGISTER_MODELCLASS( SMMVehicle );
    OZ_REGISTER_MODELCLASS( Explosion );
    OZ_REGISTER_MODELCLASS( MD2 );
    OZ_REGISTER_MODELCLASS( MD2Weapon );
    OZ_REGISTER_MODELCLASS( MD3 );

    OZ_REGISTER_AUDIOCLASS( Basic );
    OZ_REGISTER_AUDIOCLASS( Bot );
    OZ_REGISTER_AUDIOCLASS( Vehicle );

    if( library.textures.length() == 0 ) {
      throw Exception( "Context: textures missing!" );
    }
    if( library.sounds.length() == 0 ) {
      throw Exception( "Context: sounds missing!" );
    }
    if( library.bsps.length() == 0 ) {
      throw Exception( "Context: BSPs missing!" );
    }
    if( library.models.length() == 0 ) {
      throw Exception( "Context: models missing!" );
    }

    textures = new Resource<uint>[library.textures.length()];
    sounds   = new Resource<uint>[library.sounds.length()];
    bsps     = new Resource<BSP*>[library.bsps.length()];
    smms     = new Resource<SMM*>[library.models.length()];
    md2s     = new Resource<MD2*>[library.models.length()];
    md3s     = new Resource<MD3*>[library.models.length()];

    buffer.alloc( BUFFER_SIZE );

    for( int i = 0; i < library.textures.length(); ++i ) {
      textures[i].nUsers = 0;
    }
    for( int i = 0; i < library.sounds.length(); ++i ) {
      sounds[i].nUsers = 0;
    }
    for( int i = 0; i < library.bsps.length(); ++i ) {
      bsps[i].object = null;
      bsps[i].nUsers = 0;
    }
    for( int i = 0; i < library.models.length(); ++i ) {
      smms[i].object = null;
      smms[i].nUsers = 0;

      md2s[i].object = null;
      md2s[i].nUsers = 0;

      md3s[i].object = null;
      md3s[i].nUsers = 0;
    }

    maxModels     = 0;
    maxAudios     = 0;
    maxSources    = 0;
    maxBSPSources = 0;
    maxObjSources = 0;

    log.printEnd( " OK" );
  }

  void Context::free()
  {
    log.print( "Freeing Context ..." );

    buffer.dealloc();

    delete[] textures;
    delete[] sounds;
    delete[] bsps;
    delete[] smms;
    delete[] md2s;
    delete[] md3s;

    textures = null;
    sounds   = null;
    bsps     = null;
    smms     = null;
    md2s     = null;
    md3s     = null;

    modelClasses.clear();
    modelClasses.dealloc();
    audioClasses.clear();
    audioClasses.dealloc();

    log.printEnd( " OK" );
  }

#else

  bool Context::useS3TC = false;

  uint Context::buildTexture( const void* data, int width, int height, uint format,
                              bool wrap, int magFilter, int minFilter )
  {
    OZ_GL_CHECK_ERROR();

    if( useS3TC && !( Math::isPow2( width ) && Math::isPow2( height ) ) ) {
      throw Exception( "Texture must be of dimensions 2^n x 2^m to use S3 texture compression." );
    }

    bool generateMipmaps = false;
    int  internalFormat = format == GL_RGBA || format == GL_BGRA ?
         ( useS3TC ? GL_COMPRESSED_RGBA_S3TC_DXT5_EXT : GL_RGBA ) :
         ( useS3TC ? GL_COMPRESSED_RGB_S3TC_DXT1_EXT : GL_RGB );

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
        generateMipmaps = true;
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
                  format, GL_UNSIGNED_BYTE, data );

    if( generateMipmaps ) {
      glGenerateMipmap( GL_TEXTURE_2D );
    }

    glBindTexture( GL_TEXTURE_2D, 0 );

    if( glGetError() != GL_NO_ERROR || !glIsTexture( texId ) ) {
      throw Exception( "Texture building failed" );
    }

    return texId;
  }

  uint Context::createTexture( const void* data, int width, int height, uint format,
                               bool wrap, int magFilter, int minFilter )
  {
    uint texId = buildTexture( data, width, height, format, wrap, magFilter, minFilter );

    if( texId == 0 ) {
      log.println( "Error while creating texture from buffer" );
      throw Exception( "Texture loading failed" );
    }
    return texId;
  }

  uint Context::loadRawTexture( const char* path, bool wrap, int magFilter, int minFilter )
  {
    log.print( "Loading raw texture '%s' ...", path );

    const char* ext = String::findLast( path, '.' );
    if( ext == null || ( !String::equals( ext, ".png" ) && !String::equals( ext, ".jpg" ) &&
          !String::equals( ext, ".tga" ) ) )
    {
      throw Exception( "Invalid texture extension. Should be either '.png', '.jpg' or '.tga'" );
    }

    SDL_Surface* image = IMG_Load( path );
    if( image == null ) {
      throw Exception( "Texture loading failed" );
    }

    uint        format;
    const char* sFormat;

    if( image->format->BitsPerPixel == 24 ) {
      if( String::equals( ext, ".tga" ) ) {
        format = GL_BGR;
        sFormat = "BGR";
      }
      else {
        format = GL_RGB;
        sFormat = "RGB";
      }
    }
    else if( image->format->BitsPerPixel == 32 ) {
      if( String::equals( ext, ".tga" ) ) {
        format = GL_BGRA;
        sFormat = "BGRA";
      }
      else {
        format = GL_RGBA;
        sFormat = "RBGA";
      }
    }
    else {
      throw Exception( "Wrong texture format. Should be 24 bpp RGB/BGR or 32 bpp RGBA/BGRA" );
    }

    log.printEnd( " %s ... OK", sFormat );

    int bytesPerPixel = image->format->BitsPerPixel / 8;
    char* bytes = reinterpret_cast<char*>( image->pixels );

    char* pos0 = &bytes[0];
    char* pos1 = &bytes[ ( image->h - 1 ) * image->w * bytesPerPixel ];

    for( int y = 0; y < image->h / 2; ++y ) {
      for( int x = 0; x < image->w; ++x ) {
        for( int i = 0; i < bytesPerPixel; ++i ) {
          swap( pos0[i], pos1[i] );
        }

        pos0 += bytesPerPixel;
        pos1 += bytesPerPixel;
      }

      pos1 -= 2 * image->w * bytesPerPixel;
    }

    uint texId = createTexture( bytes, image->w, image->h, format, wrap,
                                magFilter, minFilter );

    SDL_FreeSurface( image );

    if( texId == 0 || !glIsTexture( texId ) ) {
      glDeleteTextures( 1, &texId );

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

      if( useS3TC ) {
        glGetTexLevelParameteriv( GL_TEXTURE_2D, i, GL_TEXTURE_COMPRESSED_IMAGE_SIZE, &size );
      }
      else {
        size = width * height * 4;
      }

      stream->writeInt( width );
      stream->writeInt( height );
      stream->writeInt( size );

      if( useS3TC ) {
        glGetCompressedTexImage( GL_TEXTURE_2D, i, stream->prepareWrite( size ) );
      }
      else {
        glGetTexImage( GL_TEXTURE_2D, i, GL_RGBA, GL_UNSIGNED_BYTE, stream->prepareWrite( size ) );
      }
    }

    OZ_GL_CHECK_ERROR();
  }

#endif

}
}
