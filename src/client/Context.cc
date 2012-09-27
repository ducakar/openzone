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
 * @file client/Context.cc
 */

#include <stable.hh>
#include <client/Context.hh>

#include <client/SMMImago.hh>
#include <client/SMMVehicleImago.hh>
#include <client/ExplosionImago.hh>
#include <client/MD2Imago.hh>
#include <client/MD2WeaponImago.hh>
#include <client/MD3Imago.hh>

#include <client/BasicAudio.hh>
#include <client/BotAudio.hh>
#include <client/VehicleAudio.hh>

#include <client/OpenGL.hh>
#include <client/OpenAL.hh>

#ifdef OZ_ESPEAK
# include <espeak/speak_lib.h>
#endif

#define OZ_REGISTER_IMAGOCLASS( name ) \
  { \
    int id = liber.imagoIndex( #name ); \
    if( id >= 0 ) { \
      imagoClasses[id] = name##Imago::create; \
    } \
  }

#define OZ_REGISTER_AUDIOCLASS( name ) \
  { \
    int id = liber.audioIndex( #name ); \
    if( id >= 0 ) { \
      audioClasses[id] = name##Audio::create; \
    } \
  }

namespace oz
{
namespace client
{

Pool<Context::Source> Context::Source::pool;
int                   Context::speakSampleRate;
Context::SpeakSource  Context::speakSource;

#ifdef OZ_ESPEAK

int Context::speakCallback( short int* samples, int nSamples, void* )
{
  if( nSamples != 0 ) {
    int maxSamples = 2 * SpeakSource::BUFFER_SIZE - speakSource.nSamples;
    if( nSamples > maxSamples ) {
      nSamples = maxSamples;
      Log::printRaw( "AL: Speak buffer overrun\n" );
    }

    memcpy( speakSource.samples + speakSource.nSamples, samples,
            size_t( nSamples ) * sizeof( short ) );
    speakSource.nSamples += nSamples;

    if( speakSource.nQueuedBuffers != 2 ) {
      int i = speakSource.nQueuedBuffers;

      speakSource.mutex.lock();

      alBufferData( speakSource.bufferIds[i], AL_FORMAT_MONO16, speakSource.samples,
                    speakSource.nSamples * int( sizeof( short ) ), speakSampleRate );
      alSourceQueueBuffers( speakSource.id, 1, &speakSource.bufferIds[i] );
      alSourcePlay( speakSource.id );

      OZ_AL_CHECK_ERROR();
      speakSource.mutex.unlock();

      ++speakSource.nQueuedBuffers;
      speakSource.nSamples = 0;
      return 0;
    }
  }

  do {
    if( speakSource.nQueuedBuffers == 0 ) {
      return 1;
    }

    int nProcessed;
    do {
      Time::sleep( 100 );

      speakSource.mutex.lock();

      int state;
      alGetSourcei( speakSource.id, AL_SOURCE_STATE, &state );
      alGetSourcei( speakSource.id, AL_BUFFERS_PROCESSED, &nProcessed );

      if( nProcessed == 0 && state == AL_STOPPED ) {
        alSourcePlay( speakSource.id );
      }

      speakSource.mutex.unlock();
    }
    while( nProcessed == 0 && speakSource.isAlive );

    if( !speakSource.isAlive ) {
      return 1;
    }

    speakSource.mutex.lock();

    ALuint buffer;
    alSourceUnqueueBuffers( speakSource.id, 1, &buffer );
    --speakSource.nQueuedBuffers;

    if( speakSource.nSamples != 0 ) {
      alBufferData( buffer, AL_FORMAT_MONO16, speakSource.samples,
                    speakSource.nSamples * int( sizeof( short ) ), speakSampleRate );
      alSourceQueueBuffers( speakSource.id, 1, &buffer );

      ++speakSource.nQueuedBuffers;
      speakSource.nSamples = 0;
    }

    OZ_AL_CHECK_ERROR();
    speakSource.mutex.unlock();
  }
  while( nSamples == 0 );

  return 0;
}

void Context::speakMain( void* )
{
  espeak_Synth( speakSource.text, size_t( speakSource.text + 1 ), 0, POS_CHARACTER, 0,
                espeakCHARS_UTF8, nullptr, nullptr );

  int value = AL_PLAYING;
  while( speakSource.isAlive && value != AL_STOPPED ) {
    Time::sleep( 100 );

    speakSource.mutex.lock();
    alGetSourcei( speakSource.id, AL_SOURCE_STATE, &value );
    OZ_AL_CHECK_ERROR();
    speakSource.mutex.unlock();
  }

  speakSource.owner   = -1;
  speakSource.isAlive = false;
}

#endif

uint Context::addSource( int sound )
{
  hard_assert( sounds[sound].nUsers > 0 );

  uint srcId;
  alGenSources( 1, &srcId );

  // Can this ever happen?
  if( srcId == INVALID_SOURCE ) {
    soft_assert( false );

    alDeleteSources( 1, &srcId );
    return INVALID_SOURCE;
  }
  if( alGetError() != AL_NO_ERROR ) {
    Log::printRaw( "AL: Too many sources\n" );
    return INVALID_SOURCE;
  }

  ++sounds[sound].nUsers;
  sources.add( new Source( srcId, sound ) );
  return srcId;
}

void Context::removeSource( Source* source, Source* prev )
{
  int sound = source->sound;

  hard_assert( sounds[sound].nUsers > 0 );

  alDeleteSources( 1, &source->id );

  --sounds[sound].nUsers;
  sources.erase( source, prev );
  delete source;
}

uint Context::addContSource( int sound, int key )
{
  hard_assert( sounds[sound].nUsers > 0 );

  uint srcId;
  alGenSources( 1, &srcId );

  // Can this ever happen?
  if( srcId == INVALID_SOURCE ) {
    soft_assert( false );

    alDeleteSources( 1, &srcId );
    return INVALID_SOURCE;
  }
  if( alGetError() != AL_NO_ERROR ) {
    Log::printRaw( "AL: Too many sources\n" );
    return INVALID_SOURCE;
  }

  ++sounds[sound].nUsers;
  contSources.add( key, ContSource( srcId, sound ) );
  return srcId;
}

void Context::removeContSource( ContSource* contSource, int key )
{
  int sound = contSource->sound;

  hard_assert( sounds[sound].nUsers > 0 );

  alDeleteSources( 1, &contSource->id );

  --sounds[sound].nUsers;
  contSources.exclude( key );
}

uint Context::requestSpeakSource( const char* text, int owner )
{
#ifdef OZ_ESPEAK

  if( speakSource.thread.isValid() ) {
    return INVALID_SOURCE;
  }

  speakSource.nQueuedBuffers = 0;
  speakSource.nSamples       = 0;
  speakSource.owner          = owner;
  speakSource.isAlive        = true;
  speakSource.text           = text;

  speakSource.thread.start( "speak", speakMain, nullptr );
  return speakSource.id;

#else

  static_cast<void>( text );
  static_cast<void>( owner );

  return INVALID_SOURCE;

#endif
}

void Context::releaseSpeakSource()
{
#ifdef OZ_ESPEAK

  hard_assert( speakSource.thread.isValid() );

  speakSource.isAlive = false;
  speakSource.thread.join();

#endif
}

Context::Context() :
  imagoClasses( nullptr ), audioClasses( nullptr ), textures( nullptr ), sounds( nullptr ),
  bsps( nullptr ), smms( nullptr ), md2s( nullptr ), md3s( nullptr )
{}

uint Context::readTextureLayer( InputStream* istream )
{
  uint texId;
  glGenTextures( 1, &texId );
  glBindTexture( GL_TEXTURE_2D, texId );

  int wrap      = istream->readInt();
  int magFilter = istream->readInt();
  int minFilter = istream->readInt();

  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap );

  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter );

  for( int level = 0; ; ++level ) {
    int width = istream->readInt();

    if( width == 0 ) {
      break;
    }

    int height = istream->readInt();
    int format = istream->readInt();
    int size   = istream->readInt();

    if( format == GL_COMPRESSED_RGB_S3TC_DXT1_EXT || format == GL_COMPRESSED_RGBA_S3TC_DXT5_EXT ) {
      glCompressedTexImage2D( GL_TEXTURE_2D, level, uint( format ), width, height, 0,
                              size, istream->forward( size ) );
    }
    else {
      glTexImage2D( GL_TEXTURE_2D, level, format, width, height, 0, uint( format ),
                    GL_UNSIGNED_BYTE, istream->forward( size ) );
    }
  }

  OZ_GL_CHECK_ERROR();

  return texId;
}

uint Context::loadTextureLayer( const char* path )
{
  PFile file( path );
  if( !file.map() ) {
    OZ_ERROR( "Texture file '%s' mmap failed", path );
  }

  InputStream is = file.inputStream();
  return readTextureLayer( &is );
}

Texture Context::readTexture( InputStream* istream )
{
  Texture texture;

  int textureFlags = istream->readInt();

  texture.diffuse = textureFlags & Mesh::DIFFUSE_BIT ? readTextureLayer( istream ) : 0;
  texture.masks   = textureFlags & Mesh::MASKS_BIT   ? readTextureLayer( istream ) :
                                                       shader.defaultMasks;
  texture.normals = textureFlags & Mesh::NORMALS_BIT ? readTextureLayer( istream ) :
                                                       shader.defaultNormals;
  return texture;
}

Texture Context::loadTexture( const char* path )
{
  PFile file( path );
  if( !file.map() ) {
    OZ_ERROR( "Texture file '%s' mmap failed", path );
  }

  InputStream is = file.inputStream();
  return readTexture( &is );
}

Texture Context::requestTexture( int id )
{
  Resource<Texture>& resource = textures[id];

  if( resource.nUsers >= 0 ) {
    ++resource.nUsers;
    return resource.id;
  }

  resource.nUsers = 1;
  resource.id = loadTexture( liber.textures[id].path );

  return resource.id;
}

void Context::releaseTexture( int id )
{
  Resource<Texture>& resource = textures[id];

  hard_assert( resource.nUsers > 0 );

  --resource.nUsers;

  if( resource.nUsers == 0 ) {
    resource.nUsers = -1;
    resource.id.free();

    OZ_GL_CHECK_ERROR();
  }
}

uint Context::requestSound( int id )
{
  Resource<uint>& resource = sounds[id];

  if( resource.nUsers >= 0 ) {
    ++resource.nUsers;
    return resource.id;
  }

  resource.nUsers = 1;

  OZ_AL_CHECK_ERROR();

  const String& name = liber.sounds[id].name;
  const String& path = liber.sounds[id].path;

  PFile file( path );
  if( !file.map() ) {
    OZ_ERROR( "Sound file '%s' mmap failed", path.cstr() );
  }

  InputStream is = file.inputStream();

  uint   length;
  ubyte* data;

  SDL_AudioSpec audioSpec;

  audioSpec.freq     = DEFAULT_AUDIO_FREQ;
  audioSpec.format   = DEFAULT_AUDIO_FORMAT;
  audioSpec.channels = 1;
  audioSpec.samples  = 0;

  SDL_RWops* rwOps = SDL_RWFromConstMem( is.begin(), is.capacity() );
  if( SDL_LoadWAV_RW( rwOps, 1, &audioSpec, &data, &length ) == nullptr ) {
    OZ_ERROR( "Failed to load WAVE sound '%s'", name.cstr() );
  }

  if( audioSpec.channels != 1 ||
      ( audioSpec.format != AUDIO_U8 && audioSpec.format != AUDIO_S16 ) )
  {
    OZ_ERROR( "Invalid sound '%s' format, should be U8 mono or S16LE mono", name.cstr() );
  }

  ALenum format = audioSpec.format == AUDIO_U8 ? AL_FORMAT_MONO8 : AL_FORMAT_MONO16;

  alGenBuffers( 1, &resource.id );
  alBufferData( resource.id, format, data, int( length ), audioSpec.freq );

  SDL_FreeWAV( data );
  file.unmap();

  OZ_AL_CHECK_ERROR();

  if( resource.id == 0 ) {
    OZ_ERROR( "Sound '%s' loading failed", name.cstr() );
  }

  return resource.id;
}

void Context::releaseSound( int id )
{
  Resource<uint>& resource = sounds[id];

  hard_assert( resource.nUsers > 0 );

  --resource.nUsers;
}

void Context::freeSound( int id )
{
  Resource<uint>& resource = sounds[id];

  hard_assert( resource.nUsers == 0 );

  --resource.nUsers;
  alDeleteBuffers( 1, &resource.id );

  OZ_AL_CHECK_ERROR();
}

SMM* Context::requestSMM( int id )
{
  Resource<SMM*>& resource = smms[id];

  if( resource.nUsers < 0 ) {
    resource.object = new SMM( id );
    resource.nUsers = 1;
  }

  ++resource.nUsers;
  return resource.object;
}

void Context::releaseSMM( int id )
{
  Resource<SMM*>& resource = smms[id];

  hard_assert( resource.object != nullptr && resource.nUsers > 0 );

  --resource.nUsers;
}

MD2* Context::requestMD2( int id )
{
  Resource<MD2*>& resource = md2s[id];

  if( resource.nUsers < 0 ) {
    resource.object = new MD2( id );
    resource.nUsers = 1;
  }

  ++resource.nUsers;
  return resource.object;
}

void Context::releaseMD2( int id )
{
  Resource<MD2*>& resource = md2s[id];

  hard_assert( resource.object != nullptr && resource.nUsers > 0 );

  --resource.nUsers;
}

MD3* Context::requestMD3( int id )
{
  Resource<MD3*>& resource = md3s[id];

  if( resource.nUsers < 0 ) {
    resource.object = new MD3( id );
    resource.nUsers = 1;
  }

  ++resource.nUsers;
  return resource.object;
}

void Context::releaseMD3( int id )
{
  Resource<MD3*>& resource = md3s[id];

  hard_assert( resource.object != nullptr && resource.nUsers > 0 );

  --resource.nUsers;
}

BSP* Context::getBSP( const Struct* str )
{
  Resource<BSP*>& resource = bsps[str->bsp->id];

  return resource.object != nullptr && resource.object->isLoaded ? resource.object : nullptr;
}

void Context::drawBSP( const Struct* str )
{
  volatile Resource<BSP*>& resource = bsps[str->bsp->id];

  // we don't count users, just to show there is at least one
  resource.nUsers = 1;

  if( resource.object == nullptr ) {
    resource.object = new BSP( str->bsp );
  }
  else if( resource.object->isLoaded ) {
    resource.object->draw( str );
  }
}

void Context::playBSP( const Struct* str )
{
  Resource<BSPAudio*>& resource = bspAudios[str->bsp->id];

  // we don't count users, just to show there is at least one
  resource.nUsers = 1;

  if( resource.object == nullptr ) {
    resource.object = new BSPAudio( str->bsp );
  }

  resource.object->play( str );
}

void Context::drawImago( const Object* obj, const Imago* parent )
{
  hard_assert( obj->flags & Object::IMAGO_BIT );

  Imago* const* value = imagines.find( obj->index );

  if( value == nullptr ) {
    Imago::CreateFunc* createFunc = imagoClasses[obj->clazz->imagoType];
    value = &imagines.add( obj->index, createFunc( obj ) );
  }

  Imago* imago = *value;
  imago->flags |= Imago::UPDATED_BIT;
  imago->draw( parent );
}

void Context::playAudio( const Object* obj, const Audio* parent )
{
  hard_assert( obj->flags & Object::AUDIO_BIT );

  Audio* const* value = audios.find( obj->index );

  if( value == nullptr ) {
    Audio::CreateFunc* createFunc = audioClasses[obj->clazz->audioType];
    value = &audios.add( obj->index, createFunc( obj ) );
  }

  Audio* audio = *value;
  audio->flags |= Audio::UPDATED_BIT;
  audio->play( parent );
}

void Context::drawFrag( const Frag* frag )
{
  FragPool* pool = fragPools[frag->poolId];

  if( pool == nullptr ) {
    pool = new FragPool( frag->pool );
    fragPools[frag->poolId] = pool;
  }

  pool->flags |= FragPool::UPDATED_BIT;
  pool->draw( frag );
}

void Context::updateLoad()
{
  int nFragPools = 0;
  for( int i = 0; i < liber.nFragPools; ++i ) {
    nFragPools += fragPools[i] != nullptr;
  }

  maxImagines           = max( maxImagines,           imagines.length() );
  maxAudios             = max( maxAudios,             audios.length() );
  maxSources            = max( maxSources,            Source::pool.length() );
  maxContSources        = max( maxContSources,        contSources.length() );

  maxSMMImagines        = max( maxSMMImagines,        SMMImago::pool.length() );
  maxSMMVehicleImagines = max( maxSMMVehicleImagines, SMMVehicleImago::pool.length() );
  maxExplosionImagines  = max( maxExplosionImagines,  ExplosionImago::pool.length() );
  maxMD2Imagines        = max( maxMD2Imagines,        MD2Imago::pool.length() );
  maxMD2WeaponImagines  = max( maxMD2WeaponImagines,  MD2WeaponImago::pool.length() );
  maxMD3Imagines        = max( maxMD3Imagines,        MD3Imago::pool.length() );

  maxBasicAudios        = max( maxBasicAudios,        BasicAudio::pool.length() );
  maxBotAudios          = max( maxBotAudios,          BotAudio::pool.length() );
  maxVehicleAudios      = max( maxVehicleAudios,      VehicleAudio::pool.length() );

  maxFragPools          = max( maxFragPools,          nFragPools );
}

void Context::load()
{
  speakSource.owner = -1;
  alGenBuffers( 2, speakSource.bufferIds );
  alGenSources( 1, &speakSource.id );
  if( alGetError() != AL_NO_ERROR ) {
    OZ_ERROR( "Failed to create speak source" );
  }

  maxImagines           = 0;
  maxAudios             = 0;
  maxSources            = 0;
  maxContSources        = 0;

  maxSMMImagines        = 0;
  maxSMMVehicleImagines = 0;
  maxExplosionImagines  = 0;
  maxMD2Imagines        = 0;
  maxMD2WeaponImagines  = 0;
  maxMD3Imagines        = 0;

  maxBasicAudios        = 0;
  maxBotAudios          = 0;
  maxVehicleAudios      = 0;

  maxFragPools          = 0;
}

void Context::unload()
{
#ifdef __native_client__
  hard_assert( NaCl::isMainThread() );
#endif

  Log::println( "Unloading Context {" );
  Log::indent();

  Log::println( "Peak instances {" );
  Log::indent();
  Log::println( "%6d  imago objects",          maxImagines );
  Log::println( "%6d  audio objects",          maxAudios );
  Log::println( "%6d  non-continuous sources", maxSources );
  Log::println( "%6d  continuous sources",     maxContSources );
  Log::println( "%6d  SMM imagines",           maxSMMImagines );
  Log::println( "%6d  SMMVehicle imagines",    maxSMMVehicleImagines );
  Log::println( "%6d  Explosion imagines",     maxExplosionImagines );
  Log::println( "%6d  MD2 imagines",           maxMD2Imagines );
  Log::println( "%6d  MD2Weapon imagines",     maxMD2WeaponImagines );
  Log::println( "%6d  MD3 imagines",           maxMD3Imagines );
  Log::println( "%6d  Basic audios",           maxBasicAudios );
  Log::println( "%6d  Bot audios",             maxBotAudios );
  Log::println( "%6d  Vehicle audios",         maxVehicleAudios );
  Log::println( "%6d  fragment pools",         maxFragPools );
  Log::unindent();
  Log::println( "}" );

  // Speak source must be destroyed before anything else using OpenAL since it calls OpenAL
  // functions from its own thread.
  if( speakSource.thread.isValid() ) {
    releaseSpeakSource();
  }
  OZ_AL_CHECK_ERROR();

  alDeleteSources( 1, &speakSource.id );
  alDeleteBuffers( 2, speakSource.bufferIds );

  imagines.free();
  imagines.deallocate();
  audios.free();
  audios.deallocate();

  aFree( fragPools, liber.nFragPools );
  aFill<FragPool*, FragPool*>( fragPools, nullptr, liber.nFragPools );

  BasicAudio::pool.free();
  BotAudio::pool.free();
  VehicleAudio::pool.free();

  OZ_AL_CHECK_ERROR();

  for( int i = 0; i < liber.nBSPs; ++i ) {
    delete bsps[i].object;

    bsps[i].object = nullptr;
    bsps[i].nUsers = -1;

    delete bspAudios[i].object;

    bspAudios[i].object = nullptr;
    bspAudios[i].nUsers = -1;
  }
  for( int i = 0; i < liber.models.length(); ++i ) {
    delete smms[i].object;
    smms[i].object = nullptr;
    smms[i].nUsers = -1;

    delete md2s[i].object;
    md2s[i].object = nullptr;
    md2s[i].nUsers = -1;

    delete md3s[i].object;
    md3s[i].object = nullptr;
    md3s[i].nUsers = -1;
  }

  SMMImago::pool.free();
  SMMVehicleImago::pool.free();
  ExplosionImago::pool.free();
  MD2Imago::pool.free();
  MD2WeaponImago::pool.free();
  MD3Imago::pool.free();

  Mesh::deallocate();

  while( !sources.isEmpty() ) {
    removeSource( sources.first(), nullptr );
    OZ_AL_CHECK_ERROR();
  }
  for( auto i = contSources.iter(); i.isValid(); ) {
    auto src = i;
    ++i;

    removeContSource( &src->value, src->key );
    OZ_AL_CHECK_ERROR();
  }

  sources.free();
  Source::pool.free();
  contSources.clear();
  contSources.deallocate();

  for( int i = 0; i < liber.textures.length(); ++i ) {
    hard_assert( textures[i].nUsers == -1 );
  }

  for( int i = 0; i < liber.sounds.length(); ++i ) {
    if( sounds[i].nUsers == 0 ) {
      freeSound( i );
    }
    hard_assert( sounds[i].nUsers == -1 );
  }

  OZ_AL_CHECK_ERROR();

  Log::unindent();
  Log::println( "}" );
}

void Context::init()
{
  Log::print( "Initialising Context ..." );

  imagoClasses = liber.nImagoClasses == 0 ? nullptr : new Imago::CreateFunc*[liber.nImagoClasses];
  audioClasses = liber.nAudioClasses == 0 ? nullptr : new Audio::CreateFunc*[liber.nAudioClasses];
  fragPools    = liber.nFragPools    == 0 ? nullptr : new FragPool*[liber.nFragPools];

  OZ_REGISTER_IMAGOCLASS( SMM );
  OZ_REGISTER_IMAGOCLASS( SMMVehicle );
  OZ_REGISTER_IMAGOCLASS( Explosion );
  OZ_REGISTER_IMAGOCLASS( MD2 );
  OZ_REGISTER_IMAGOCLASS( MD2Weapon );
  OZ_REGISTER_IMAGOCLASS( MD3 );

  OZ_REGISTER_AUDIOCLASS( Basic );
  OZ_REGISTER_AUDIOCLASS( Bot );
  OZ_REGISTER_AUDIOCLASS( Vehicle );

  aFill<FragPool*, FragPool*>( fragPools, nullptr, liber.nFragPools );

  int nTextures = liber.textures.length();
  int nSounds   = liber.sounds.length();
  int nBSPs     = liber.nBSPs;
  int nModels   = liber.models.length();

  textures  = nTextures == 0 ? nullptr : new Resource<Texture>[nTextures];
  sounds    = nSounds   == 0 ? nullptr : new Resource<uint>[nSounds];

  bsps      = nBSPs     == 0 ? nullptr : new Resource<BSP*>[nBSPs];
  bspAudios = nBSPs     == 0 ? nullptr : new Resource<BSPAudio*>[nBSPs];

  smms      = nModels   == 0 ? nullptr : new Resource<SMM*>[nModels];
  md2s      = nModels   == 0 ? nullptr : new Resource<MD2*>[nModels];
  md3s      = nModels   == 0 ? nullptr : new Resource<MD3*>[nModels];

  for( int i = 0; i < nTextures; ++i ) {
    textures[i].nUsers = -1;
  }
  for( int i = 0; i < nSounds; ++i ) {
    sounds[i].nUsers = -1;
  }
  for( int i = 0; i < nBSPs; ++i ) {
    bsps[i].object = nullptr;
    bsps[i].nUsers = -1;

    bspAudios[i].object = nullptr;
    bspAudios[i].nUsers = -1;
  }
  for( int i = 0; i < nModels; ++i ) {
    smms[i].object = nullptr;
    smms[i].nUsers = -1;

    md2s[i].object = nullptr;
    md2s[i].nUsers = -1;

    md3s[i].object = nullptr;
    md3s[i].nUsers = -1;
  }

  speakSource.mutex.init();

  Log::printEnd( " OK" );
}

void Context::free()
{
  Log::print( "Freeing Context ..." );

  speakSource.mutex.destroy();

  delete[] imagoClasses;
  delete[] audioClasses;
  delete[] fragPools;

  delete[] textures;
  delete[] sounds;

  delete[] bsps;
  delete[] bspAudios;

  delete[] smms;
  delete[] md2s;
  delete[] md3s;

  imagoClasses = nullptr;
  audioClasses = nullptr;
  fragPools    = nullptr;

  textures     = nullptr;
  sounds       = nullptr;

  bsps         = nullptr;
  bspAudios    = nullptr;

  smms         = nullptr;
  md2s         = nullptr;
  md3s         = nullptr;

  Log::printEnd( " OK" );
}

Context context;

}
}
