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
    hard_assert( glGetError() == GL_NO_ERROR );

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

      hard_assert( glGetError() == GL_NO_ERROR );
    }
    return texNum;
  }

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

  uint Context::createTexture( const void* data, int width, int height, int bytesPerPixel,
                               bool wrap, int magFilter, int minFilter )
  {
    uint texNum = buildTexture( data, width, height, bytesPerPixel, wrap, magFilter, minFilter );

    if( texNum == ~0u ) {
      log.println( "Error creating texture from buffer" );
    }
    return texNum;
  }

  uint Context::loadTexture( const char* path, bool wrap, int magFilter, int minFilter )
  {
    log.print( "Loading texture '%s' ...", path );

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

  void Context::deleteTexture( uint id )
  {
    glDeleteTextures( 1, &id );
    hard_assert( glGetError() == GL_NO_ERROR );
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

    hard_assert( alGetError() == AL_NO_ERROR );

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

    hard_assert( alGetError() == AL_NO_ERROR );

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

  BSP* Context::loadBSP( int id )
  {
    Resource<BSP*>& resource = bsps[id];

    hard_assert( resource.object == null && resource.nUsers == 0 );

    resource.object = new BSP( id );
    return resource.object;
  }

  void Context::releaseBSP( int id )
  {
    Resource<BSP*>& resource = bsps[id];

    hard_assert( resource.object != null );

    delete resource.object;
    resource.object = null;
    resource.nUsers = 0;
  }

  void Context::beginArrayMode()
  {
    glActiveTexture( GL_TEXTURE0 );
    glEnable( GL_TEXTURE_2D );

    glActiveTexture( GL_TEXTURE1 );
    glEnable( GL_TEXTURE_2D );

    glActiveTexture( GL_TEXTURE2 );
    glEnable( GL_TEXTURE_2D );

    glEnableClientState( GL_VERTEX_ARRAY );
    glEnableClientState( GL_NORMAL_ARRAY );

    glClientActiveTexture( GL_TEXTURE0 );
    glEnableClientState( GL_TEXTURE_COORD_ARRAY );

    glClientActiveTexture( GL_TEXTURE1 );
    glEnableClientState( GL_TEXTURE_COORD_ARRAY );

    glClientActiveTexture( GL_TEXTURE2 );
    glEnableClientState( GL_TEXTURE_COORD_ARRAY );
  }

  void Context::endArrayMode()
  {
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );

    glDisableClientState( GL_VERTEX_ARRAY );
    glDisableClientState( GL_NORMAL_ARRAY );

    glClientActiveTexture( GL_TEXTURE0 );
    glDisableClientState( GL_TEXTURE_COORD_ARRAY );

    glClientActiveTexture( GL_TEXTURE1 );
    glDisableClientState( GL_TEXTURE_COORD_ARRAY );

    glClientActiveTexture( GL_TEXTURE2 );
    glDisableClientState( GL_TEXTURE_COORD_ARRAY );

    glActiveTexture( GL_TEXTURE0 );
    glDisable( GL_TEXTURE_2D );

    glActiveTexture( GL_TEXTURE1 );
    glDisable( GL_TEXTURE_2D );

    glActiveTexture( GL_TEXTURE2 );
    glDisable( GL_TEXTURE_2D );

    glActiveTexture( GL_TEXTURE0 );
    glEnable( GL_TEXTURE_2D );
  }

  uint Context::genArray( GLenum usage,
                          const Vertex* vertices, int nVertices,
                          const ushort* indices, int nIndices )
  {
    hard_assert( ( indices == null ) == ( nIndices == 0 ) );

    int  id  = vaos.add();
    VAO& vao = vaos[id];

    vao.usage = usage;

    if( nIndices != 0 ) {
      glGenBuffers( 2, vao.buffers );
    }
    else {
      glGenBuffers( 1, vao.buffers );
      vao.buffers[1] = 0;
    }

    glBindBuffer( GL_ARRAY_BUFFER, vao.buffers[0] );
    glBufferData( GL_ARRAY_BUFFER, nVertices * sizeof( Vertex ), vertices, usage );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );

    if( nIndices != 0 ) {
      glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vao.buffers[1] );
      glBufferData( GL_ELEMENT_ARRAY_BUFFER, nIndices * sizeof( ushort ), indices, usage );
      glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
    }

    return id;
  }

  void Context::deleteArray( uint id )
  {
    glDeleteBuffers( 2, vaos[id].buffers );
    vaos.remove( id );
  }

  void Context::bindArray( uint id ) const
  {
    const VAO& vao = vaos[id];

    glBindBuffer( GL_ARRAY_BUFFER, vao.buffers[0] );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vao.buffers[1] );

    glVertexPointer( 3, GL_FLOAT, sizeof( Vertex ),
                     reinterpret_cast<const char*>( 0 ) + offsetof( Vertex, pos ) );

    glNormalPointer( GL_FLOAT, sizeof( Vertex ),
                     reinterpret_cast<const char*>( 0 ) + offsetof( Vertex, normal ) );

    glClientActiveTexture( GL_TEXTURE0 );
    glTexCoordPointer( 2, GL_FLOAT, sizeof( Vertex ),
                       reinterpret_cast<const char*>( 0 ) + offsetof( Vertex, texCoord ) );

    glClientActiveTexture( GL_TEXTURE1 );
    glTexCoordPointer( 2, GL_FLOAT, sizeof( Vertex ),
                       reinterpret_cast<const char*>( 0 ) + offsetof( Vertex, texCoord ) );

    glClientActiveTexture( GL_TEXTURE2 );
    glTexCoordPointer( 2, GL_FLOAT, sizeof( Vertex ),
                       reinterpret_cast<const char*>( 0 ) + offsetof( Vertex, texCoord ) );
  }

  void Context::setVertexFormat()
  {
    glVertexPointer( 3, GL_FLOAT, sizeof( Vertex ),
                     reinterpret_cast<const char*>( 0 ) + offsetof( Vertex, pos ) );

    glNormalPointer( GL_FLOAT, sizeof( Vertex ),
                     reinterpret_cast<const char*>( 0 ) + offsetof( Vertex, normal ) );

    glClientActiveTexture( GL_TEXTURE0 );
    glTexCoordPointer( 2, GL_FLOAT, sizeof( Vertex ),
                       reinterpret_cast<const char*>( 0 ) + offsetof( Vertex, texCoord ) );

    glClientActiveTexture( GL_TEXTURE1 );
    glTexCoordPointer( 2, GL_FLOAT, sizeof( Vertex ),
                       reinterpret_cast<const char*>( 0 ) + offsetof( Vertex, texCoord ) );

    glClientActiveTexture( GL_TEXTURE2 );
    glTexCoordPointer( 2, GL_FLOAT, sizeof( Vertex ),
                       reinterpret_cast<const char*>( 0 ) + offsetof( Vertex, texCoord ) );
  }

  void Context::uploadArray( uint id, const Vertex* vertices, int nVertices ) const
  {
    const VAO& vao = vaos[id];

    glBindBuffer( GL_ARRAY_BUFFER, vao.buffers[0] );
    glBufferData( GL_ARRAY_BUFFER, nVertices * sizeof( Vertex ), vertices, vao.usage );
  }

  Vertex* Context::mapArray( uint id, int access, int size ) const
  {
    const VAO& vao = vaos[id];

    glBindBuffer( GL_ARRAY_BUFFER, vao.buffers[0] );

    if( size != 0 ) {
      glBufferData( GL_ARRAY_BUFFER, size, null, vao.usage );
    }

    return reinterpret_cast<Vertex*>( glMapBuffer( GL_ARRAY_BUFFER, access ) );
  }

  void Context::unmapArray( uint id ) const
  {
    const VAO& vao = vaos[id];

    glBindBuffer( GL_ARRAY_BUFFER, vao.buffers[0] );
    glUnmapBuffer( GL_ARRAY_BUFFER );
  }

  void Context::drawArray( GLenum mode, int firstVertex, int nVertices ) const
  {
    glDrawArrays( mode, firstVertex, nVertices );
  }

  void Context::drawIndexedArray( GLenum mode, int firstIndex, int nIndices ) const
  {
    glDrawElements( mode, nIndices, GL_UNSIGNED_SHORT,
                    reinterpret_cast<const ushort*>( 0 ) + firstIndex );
  }

  void Context::bindTextures( uint texture0, uint texture1, uint texture2 )
  {
    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, texture0 );
    glActiveTexture( GL_TEXTURE1 );
    glBindTexture( GL_TEXTURE_2D, texture1 );
    glActiveTexture( GL_TEXTURE2 );
    glBindTexture( GL_TEXTURE_2D, texture2 );
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

  void Context::deleteLists( uint listId )
  {
    for( int i = 0; i < lists.length(); ++i ) {
      if( lists[i].base == listId ) {
        glDeleteLists( lists[i].base, lists[i].count );
        lists.remove( i );
        break;
      }
    }
  }

  SMM* Context::loadSMM( const char* name )
  {
    Resource<SMM*>* resource = smms.find( name );

    if( resource == null ) {
      resource = smms.add( name, Resource<SMM*>() );
      resource->object = new SMM( name );
      resource->nUsers = 0;
    }

    ++resource->nUsers;
    return resource->object;
  }

  void Context::releaseSMM( const char* name )
  {
    Resource<SMM*>* resource = smms.find( name );

    hard_assert( resource != null && resource->nUsers > 0 );

    --resource->nUsers;
  }

  MD2* Context::loadStaticMD2( const char* path )
  {
    Resource<MD2*>* resource = staticMd2s.find( path );

    if( resource == null ) {
      resource = staticMd2s.add( path, Resource<MD2*>() );
      resource->object = new MD2( path );
      resource->nUsers = 0;
    }

    ++resource->nUsers;
    return resource->object;
  }

  void Context::releaseStaticMD2( const char* path )
  {
    Resource<MD2*>* resource = staticMd2s.find( path );

    hard_assert( resource != null && resource->nUsers > 0 );

    --resource->nUsers;
  }

  MD2* Context::loadMD2( const char* path )
  {
    Resource<MD2*>* resource = md2s.find( path );

    if( resource == null ) {
      resource = md2s.add( path, Resource<MD2*>() );
      resource->object = new MD2( path );
      resource->nUsers = 0;
    }

    ++resource->nUsers;
    return resource->object;
  }

  void Context::releaseMD2( const char* path )
  {
    Resource<MD2*>* resource = md2s.find( path );

    hard_assert( resource != null && resource->nUsers > 0 );

    --resource->nUsers;
  }

  MD3* Context::loadStaticMD3( const char* path )
  {
    Resource<MD3*>* resource = staticMd3s.find( path );

    if( resource == null ) {
      resource = staticMd3s.add( path, Resource<MD3*>() );
      resource->object = new MD3( path );
      resource->nUsers = 0;
    }

    ++resource->nUsers;
    return resource->object;
  }

  void Context::releaseStaticMD3( const char* path )
  {
    Resource<MD3*>* resource = staticMd3s.find( path );

    hard_assert( resource != null && resource->nUsers > 0 );

    --resource->nUsers;
  }

  MD3* Context::loadMD3( const char* path )
  {
    Resource<MD3*>* resource = md3s.find( path );

    if( resource == null ) {
      resource = md3s.add( path, Resource<MD3*>() );
      resource->object = new MD3( path );
      resource->nUsers = 0;
    }

    ++resource->nUsers;
    return resource->object;
  }

  void Context::releaseMD3( const char* path )
  {
    Resource<MD3*>* resource = md3s.find( path );

    hard_assert( resource != null && resource->nUsers > 0 );

    --resource->nUsers;
  }

  void Context::drawBSP( const Struct* str, int mask )
  {
    Resource<BSP*>& resource = bsps[str->bsp];

    if( resource.object == null ) {
      loadBSP( str->bsp );
    }
    else if( resource.object->isLoaded ) {
      // we don't count users, just to show there is at least one
      resource.nUsers = 1;
      resource.object->draw( str, mask );
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
        throw Exception( "Invalid Model" );
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
        throw Exception( "Invalid Audio" );
      }

      value = audios.add( obj->index, ( *createFunc )( obj ) );
    }

    Audio* audio = *value;

    audio->flags |= Audio::UPDATED_BIT;
    audio->play( parent );
  }

  void Context::updateLoad()
  {
    maxModels      = max( maxModels, models.length() );
    maxAudios      = max( maxAudios, audios.length() );
    maxSources     = max( maxSources, sources.length() );
    maxContSources = max( maxContSources, contSources.length() );
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
    log.println( "ContSources  %d (hashtable load %.2f)", maxContSources,
                 float( maxContSources ) / float( contSources.capacity() ) );
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

    maxModels      = 0;
    maxAudios      = 0;
    maxSources     = 0;
    maxContSources = 0;

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

    foreach( i, smms.citer() ) {
      hard_assert( i->nUsers == 0 );

      delete i->object;
      smms.exclude( i.key() );
    }
    foreach( i, staticMd2s.citer() ) {
      hard_assert( i->nUsers == 0 );

      delete i->object;
      staticMd2s.exclude( i.key() );
    }
    foreach( i, md2s.citer() ) {
      hard_assert( i->nUsers == 0 );

      delete i->object;
      md2s.exclude( i.key() );
    }
    foreach( i, staticMd3s.citer() ) {
      hard_assert( i->nUsers == 0 );

      delete i->object;
      staticMd3s.exclude( i.key() );
    }
    foreach( i, md3s.citer() ) {
      hard_assert( i->nUsers == 0 );

      delete i->object;
      md3s.exclude( i.key() );
    }
    for( int i = 0; i < translator.bsps.length(); ++i ) {
      delete bsps[i].object;
      bsps[i].object = null;
      bsps[i].nUsers = 0;
    }

    hard_assert( vaos.isEmpty() );

    vaos.dealloc();
    lists.clear();
    lists.dealloc();
    smms.dealloc();
    staticMd2s.dealloc();
    md2s.dealloc();
    staticMd3s.dealloc();
    md3s.dealloc();

    hard_assert( lists.length() == 0 );

    foreach( src, sources.citer() ) {
      alSourceStop( src->source );
      alDeleteSources( 1, &src->source );
      hard_assert( alGetError() == AL_NO_ERROR );
    }
    foreach( src, contSources.citer() ) {
      alSourceStop( src->source );
      alDeleteSources( 1, &src->source );
      hard_assert( alGetError() == AL_NO_ERROR );
    }

    sources.free();
    contSources.clear();
    contSources.dealloc();

    for( int i = 0; i < translator.textures.length(); ++i ) {
      hard_assert( textures[i].nUsers == 0 );
    }
    for( int i = 0; i < translator.sounds.length(); ++i ) {
      hard_assert( sounds[i].nUsers <= 0 );

      if( sounds[i].nUsers != -1 ) {
        alDeleteBuffers( 1, &sounds[i].id );
        hard_assert( alGetError() == AL_NO_ERROR );
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

    log.unindent();
    log.println( "}" );
  }

  void Context::init()
  {
    log.print( "Initialising Context ..." );

    OZ_REGISTER_MODELCLASS( SMM );
    OZ_REGISTER_MODELCLASS( SMMVehicle );
    OZ_REGISTER_MODELCLASS( Explosion );
    OZ_REGISTER_MODELCLASS( MD2 );
    OZ_REGISTER_MODELCLASS( MD2Weapon );

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
