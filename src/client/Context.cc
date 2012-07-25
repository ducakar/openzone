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

#include "stable.hh"

#include "client/Context.hh"

#include "client/SMMImago.hh"
#include "client/SMMVehicleImago.hh"
#include "client/ExplosionImago.hh"
#include "client/MD2Imago.hh"
#include "client/MD2WeaponImago.hh"
#include "client/MD3Imago.hh"

#include "client/BasicAudio.hh"
#include "client/BotAudio.hh"
#include "client/VehicleAudio.hh"

#include "client/OpenGL.hh"
#include "client/OpenAL.hh"

#define OZ_REGISTER_IMAGOCLASS( name ) \
  { \
    int id = library.imagoIndex( #name ); \
    if( id >= 0 ) { \
      imagoClasses[id] = name##Imago::create; \
    } \
  }

#define OZ_REGISTER_AUDIOCLASS( name ) \
  { \
    int id = library.audioIndex( #name ); \
    if( id >= 0 ) { \
      audioClasses[id] = name##Audio::create; \
    } \
  }

namespace oz
{
namespace client
{

Context context;

Pool<Context::Source> Context::Source::pool;

void Context::addSource( uint srcId, int sound )
{
  hard_assert( sounds[sound].nUsers > 0 );

  ++sounds[sound].nUsers;
  sources.add( new Source( srcId, sound ) );
}

void Context::removeSource( Source* source, Source* prev )
{
  int sound = source->sound;

  hard_assert( sounds[sound].nUsers > 0 );

  --sounds[sound].nUsers;
  sources.remove( source, prev );
  delete source;
}

void Context::addContSource( uint srcId, int sound, int key )
{
  hard_assert( sounds[sound].nUsers > 0 );

  ++sounds[sound].nUsers;
  contSources.add( key, ContSource( srcId, sound ) );
}

void Context::removeContSource( ContSource* contSource, int key )
{
  int sound = contSource->sound;

  hard_assert( sounds[sound].nUsers > 0 );

  --sounds[sound].nUsers;
  contSources.exclude( key );
}

Context::Context() :
  imagoClasses( null ), audioClasses( null ), textures( null ), sounds( null ), bsps( null ),
  smms( null ), md2s( null ), md3s( null )
{}

uint Context::readTextureLayer( InputStream* stream_, const char* path_ )
{
  static InputStream* stream;
  static const char*  path;
  static uint         texId;

  stream = stream_;
  path   = path_;

  OZ_GL_CHECK_ERROR();

  glGenTextures( 1, &texId );
  glBindTexture( GL_TEXTURE_2D, texId );

  int wrap      = stream->readInt();
  int magFilter = stream->readInt();
  int minFilter = stream->readInt();

  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap );

  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter );

  for( int level = 0; ; ++level ) {
    int width = stream->readInt();

    if( width == 0 ) {
      break;
    }

    int height = stream->readInt();
    int format = stream->readInt();
    int size   = stream->readInt();

    if( format == GL_COMPRESSED_RGB_S3TC_DXT1_EXT || format == GL_COMPRESSED_RGBA_S3TC_DXT5_EXT )
    {
      glCompressedTexImage2D( GL_TEXTURE_2D, level, uint( format ), width, height, 0,
                              size, stream->forward( size ) );
    }
    else {
      glTexImage2D( GL_TEXTURE_2D, level, format, width, height, 0, uint( format ),
                    GL_UNSIGNED_BYTE, stream->forward( size ) );
    }
  }

  OZ_GL_CHECK_ERROR();

  return texId;
}

uint Context::loadTextureLayer( const char* path )
{
  PFile file( path );
  if( !file.map() ) {
    throw Exception( "Texture file '%s' mmap failed", path );
  }

  InputStream is = file.inputStream();

  uint id = readTextureLayer( &is, path );

  file.unmap();
  return id;
}

Texture Context::loadTexture( const char* path )
{
  Texture texture;

  PFile file( path );
  if( !file.map() ) {
    throw Exception( "Texture file '%s' mmap failed", path );
  }

  InputStream is = file.inputStream();

  int textureFlags = is.readInt();

  texture.diffuse = textureFlags & Mesh::DIFFUSE_BIT ? readTextureLayer( &is, path ) : 0;
  texture.masks   = textureFlags & Mesh::MASKS_BIT   ? readTextureLayer( &is, path ) : shader.defaultMasks;
  texture.normals = textureFlags & Mesh::NORMALS_BIT ? readTextureLayer( &is, path ) : shader.defaultNormals;

  file.unmap();
  return texture;
}

Texture Context::requestTexture( int id )
{
  Resource<Texture>& resource = textures[id];

  if( resource.nUsers >= 0 ) {
    ++resource.nUsers;
    return resource.id;
  }

  resource.nUsers = 1;
  resource.id = loadTexture( library.textures[id].path );

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

  const String& name = library.sounds[id].name;
  const String& path = library.sounds[id].path;

  PFile file( path );
  if( !file.map() ) {
    throw Exception( "Sound file '%s' mmap failed", path.cstr() );
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
  if( SDL_LoadWAV_RW( rwOps, 1, &audioSpec, &data, &length ) == null ) {
    throw Exception( "Failed to load WAVE sound '%s'", name.cstr() );
  }

  if( audioSpec.channels != 1 ||
      ( audioSpec.format != AUDIO_U8 && audioSpec.format != AUDIO_S16 ) )
  {
    throw Exception( "Invalid sound '%s' format, should be U8 mono or S16LE mono", name.cstr() );
  }

  ALenum format = audioSpec.format == AUDIO_U8 ? AL_FORMAT_MONO8 : AL_FORMAT_MONO16;

  alGenBuffers( 1, &resource.id );
  alBufferData( resource.id, format, data, int( length ), audioSpec.freq );

  SDL_FreeWAV( data );
  file.unmap();

  OZ_AL_CHECK_ERROR();

  if( resource.id == 0 ) {
    throw Exception( "Sound '%s' loading failed", name.cstr() );
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

  hard_assert( resource.object != null && resource.nUsers > 0 );

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

  hard_assert( resource.object != null && resource.nUsers > 0 );

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

  hard_assert( resource.object != null && resource.nUsers > 0 );

  --resource.nUsers;
}

BSP* Context::getBSP( const Struct* str )
{
  Resource<BSP*>& resource = bsps[str->bsp->id];

  return resource.object != null && resource.object->isLoaded ? resource.object : null;
}

void Context::drawBSP( const Struct* str )
{
  volatile Resource<BSP*>& resource = bsps[str->bsp->id];

  // we don't count users, just to show there is at least one
  resource.nUsers = 1;

  if( resource.object == null ) {
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

  if( resource.object == null ) {
    resource.object = new BSPAudio( str->bsp );
  }

  resource.object->play( str );
}

void Context::drawImago( const Object* obj, const Imago* parent )
{
  hard_assert( obj->flags & Object::IMAGO_BIT );

  Imago* const* value = imagines.find( obj->index );

  if( value == null ) {
    Imago::CreateFunc* createFunc = imagoClasses[obj->clazz->imagoType];

    value = imagines.add( obj->index, createFunc( obj ) );
  }

  Imago* imago = *value;

  imago->draw( parent );

  hard_assert( imago->flags & Imago::UPDATED_BIT );
}

void Context::playAudio( const Object* obj, const Audio* parent )
{
  hard_assert( obj->flags & Object::AUDIO_BIT );

  Audio* const* value = audios.find( obj->index );

  if( value == null ) {
    Audio::CreateFunc* createFunc = audioClasses[obj->clazz->audioType];

    value = audios.add( obj->index, createFunc( obj ) );
  }

  Audio* audio = *value;

  audio->play( parent );

  hard_assert( audio->flags & Audio::UPDATED_BIT );
}

void Context::drawFrag( const Frag* frag )
{
  FragPool* pool = fragPools[frag->poolId];

  if( pool == null ) {
    pool = new FragPool( frag->pool );

    fragPools[frag->poolId] = pool;
  }

  pool->draw( frag );

  hard_assert( pool->flags & FragPool::UPDATED_BIT );
}

void Context::updateLoad()
{
  int nFragPools = 0;
  for( int i = 0; i < library.nFragPools; ++i ) {
    nFragPools += fragPools[i] != null;
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

  for( int i = 0; i < library.textures.length(); ++i ) {
    hard_assert( textures[i].nUsers == -1 );
  }
  for( int i = 0; i < library.sounds.length(); ++i ) {
    hard_assert( sounds[i].nUsers == -1 );
  }
  for( int i = 0; i < library.nBSPs; ++i ) {
    hard_assert( bsps[i].nUsers == -1 );
  }
  for( int i = 0; i < library.models.length(); ++i ) {
    hard_assert( smms[i].nUsers == -1 );
    hard_assert( md2s[i].nUsers == -1 );
    hard_assert( md3s[i].nUsers == -1 );
  }
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

  imagines.free();
  imagines.dealloc();
  audios.free();
  audios.dealloc();

  aFree( fragPools, library.nFragPools );
  aSet<FragPool*, FragPool*>( fragPools, null, library.nFragPools );

  BasicAudio::pool.free();
  BotAudio::pool.free();
  VehicleAudio::pool.free();

  OZ_AL_CHECK_ERROR();

  for( int i = 0; i < library.nBSPs; ++i ) {
    delete bsps[i].object;

    bsps[i].object = null;
    bsps[i].nUsers = -1;

    delete bspAudios[i].object;

    bspAudios[i].object = null;
    bspAudios[i].nUsers = -1;
  }
  for( int i = 0; i < library.models.length(); ++i ) {
    delete smms[i].object;

    smms[i].object = null;
    smms[i].nUsers = -1;

    delete md2s[i].object;

    md2s[i].object = null;
    md2s[i].nUsers = -1;

    delete md3s[i].object;

    md3s[i].object = null;
    md3s[i].nUsers = -1;
  }

  SMMImago::pool.free();
  SMMVehicleImago::pool.free();
  ExplosionImago::pool.free();
  MD2Imago::pool.free();
  MD2WeaponImago::pool.free();
  MD3Imago::pool.free();

  Mesh::dealloc();

  while( !sources.isEmpty() ) {
    alDeleteSources( 1, &sources.first()->id );
    removeSource( sources.first(), null );
    OZ_AL_CHECK_ERROR();
  }
  for( auto i = contSources.iter(); i.isValid(); ) {
    auto src = i;
    ++i;

    alDeleteSources( 1, &src.value().id );
    removeContSource( &src.value(), src.key() );
    OZ_AL_CHECK_ERROR();
  }

  sources.free();
  Source::pool.free();
  contSources.clear();
  contSources.dealloc();

  for( int i = 0; i < library.textures.length(); ++i ) {
    hard_assert( textures[i].nUsers == -1 );
  }

  for( int i = 0; i < library.sounds.length(); ++i ) {
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

  imagoClasses = library.nImagoClasses == 0 ? null : new Imago::CreateFunc*[library.nImagoClasses];
  audioClasses = library.nAudioClasses == 0 ? null : new Audio::CreateFunc*[library.nAudioClasses];
  fragPools    = library.nFragPools    == 0 ? null : new FragPool*[library.nFragPools];

  OZ_REGISTER_IMAGOCLASS( SMM );
  OZ_REGISTER_IMAGOCLASS( SMMVehicle );
  OZ_REGISTER_IMAGOCLASS( Explosion );
  OZ_REGISTER_IMAGOCLASS( MD2 );
  OZ_REGISTER_IMAGOCLASS( MD2Weapon );
  OZ_REGISTER_IMAGOCLASS( MD3 );

  OZ_REGISTER_AUDIOCLASS( Basic );
  OZ_REGISTER_AUDIOCLASS( Bot );
  OZ_REGISTER_AUDIOCLASS( Vehicle );

  aSet<FragPool*, FragPool*>( fragPools, null, library.nFragPools );

  int nTextures = library.textures.length();
  int nSounds   = library.sounds.length();
  int nBSPs     = library.nBSPs;
  int nModels   = library.models.length();

  textures  = nTextures == 0 ? null : new Resource<Texture>[nTextures];
  sounds    = nSounds   == 0 ? null : new Resource<uint>[nSounds];

  bsps      = nBSPs     == 0 ? null : new Resource<BSP*>[nBSPs];
  bspAudios = nBSPs     == 0 ? null : new Resource<BSPAudio*>[nBSPs];

  smms      = nModels   == 0 ? null : new Resource<SMM*>[nModels];
  md2s      = nModels   == 0 ? null : new Resource<MD2*>[nModels];
  md3s      = nModels   == 0 ? null : new Resource<MD3*>[nModels];

  for( int i = 0; i < nTextures; ++i ) {
    textures[i].nUsers = -1;
  }
  for( int i = 0; i < nSounds; ++i ) {
    sounds[i].nUsers = -1;
  }
  for( int i = 0; i < nBSPs; ++i ) {
    bsps[i].object = null;
    bsps[i].nUsers = -1;

    bspAudios[i].object = null;
    bspAudios[i].nUsers = -1;
  }
  for( int i = 0; i < nModels; ++i ) {
    smms[i].object = null;
    smms[i].nUsers = -1;

    md2s[i].object = null;
    md2s[i].nUsers = -1;

    md3s[i].object = null;
    md3s[i].nUsers = -1;
  }

  Log::printEnd( " OK" );
}

void Context::free()
{
  Log::print( "Freeing Context ..." );

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

  imagoClasses = null;
  audioClasses = null;
  fragPools    = null;

  textures     = null;
  sounds       = null;

  bsps         = null;
  bspAudios    = null;

  smms         = null;
  md2s         = null;
  md3s         = null;

  Log::printEnd( " OK" );
}

}
}
