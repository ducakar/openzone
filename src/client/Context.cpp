/*
 * OpenZone - Simple Cross-Platform FPS/RTS Game Engine
 * Copyright (C) 2002-2011  Davorin Učakar
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
 *
 * Davorin Učakar <davorin.ucakar@gmail.com>
 */

/**
 * @file client/Context.cpp
 */

#include "stable.hpp"

#include "client/Context.hpp"

#include "client/MD2.hpp"
#include "client/MD3.hpp"

#include "client/SMMImago.hpp"
#include "client/SMMVehicleImago.hpp"
#include "client/ExplosionImago.hpp"
#include "client/MD2Imago.hpp"
#include "client/MD2WeaponImago.hpp"
#include "client/MD3Imago.hpp"

#include "client/BasicAudio.hpp"
#include "client/BotAudio.hpp"
#include "client/VehicleAudio.hpp"

#include "client/OpenGL.hpp"
#include "client/OpenAL.hpp"

#define OZ_REGISTER_IMAGOCLASS( name ) \
  { \
    int id = library.imagoIndex( #name ); \
    if( id != -1 ) { \
      imagoClasses[id] = name##Imago::create; \
    } \
  }

#define OZ_REGISTER_AUDIOCLASS( name ) \
  { \
    int id = library.audioIndex( #name ); \
    if( id != -1 ) { \
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

void Context::addBSPSource( uint srcId, int sound, int key )
{
  hard_assert( sounds[sound].nUsers > 0 );

  ++sounds[sound].nUsers;
  bspSources.add( key, ContSource( srcId, sound ) );
}

void Context::addObjSource( uint srcId, int sound, int key )
{
  hard_assert( sounds[sound].nUsers > 0 );

  ++sounds[sound].nUsers;
  objSources.add( key, ContSource( srcId, sound ) );
}

void Context::removeSource( Source* source, Source* prev )
{
  int sound = source->sound;

  hard_assert( sounds[sound].nUsers > 0 );

  sources.remove( source, prev );
  delete source;
  releaseSound( sound );
}

void Context::removeBSPSource( ContSource* contSource, int key )
{
  int sound = contSource->sound;

  hard_assert( sounds[sound].nUsers > 0 );

  bspSources.exclude( key );
  releaseSound( sound );
}

void Context::removeObjSource( ContSource* contSource, int key )
{
  int sound = contSource->sound;

  hard_assert( sounds[sound].nUsers > 0 );

  objSources.exclude( key );
  releaseSound( sound );
}

Context::Context() : imagoClasses( null ), audioClasses( null ),
    textures( null ), sounds( null ), bsps( null ), smms( null ), md2s( null ), md3s( null )
{}

uint Context::loadTexture( const char* path )
{
  log.print( "Loading texture '%s' ...", path );

  File file( path );
  if( !file.map() ) {
    throw Exception( "Texture file mmap failed" );
  }

  InputStream is = file.inputStream();
  uint id = readTexture( &is );

  file.unmap();

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

  if( !shader.hasS3TC && usesS3TC ) {
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
                              size, stream->forward( size ) );
    }
    else {
      glTexImage2D( GL_TEXTURE_2D, i, internalFormat, width, height, 0,
                    GL_RGBA, GL_UNSIGNED_BYTE, stream->forward( size ) );
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

  File file( "bsp/" + name + ".ozcTex" );
  if( !file.map() ) {
    throw Exception( "Texture file mmap failed" );
  }

  InputStream is = file.inputStream();

  resource.id = readTexture( &is );

  file.unmap();

  if( resource.id == 0 ) {
    throw Exception( "Texture loading failed" );
  }

  log.printEnd( " OK" );

  return resource.id;
}

void Context::releaseTexture( int id )
{
  Resource<uint>& resource = textures[id];

  hard_assert( resource.nUsers > 0 );

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

  if( resource.nUsers != -1 ) {
    resource.nUsers = max( resource.nUsers + 1, 1 );
    return resource.id;
  }

  resource.nUsers = 1;

  OZ_AL_CHECK_ERROR();

  const String& name = library.sounds[id].name;
  const String& path = library.sounds[id].path;

  log.print( "Loading sound '%s' ...", name.cstr() );

  File file( path );
  if( !file.map() ) {
    throw Exception( "Failed to mmap sound file" );
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
  file.unmap();

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

  hard_assert( resource.nUsers > 0 );

  --resource.nUsers;
}

void Context::freeSound( int id )
{
  Resource<uint>& resource = sounds[id];

  hard_assert( resource.nUsers == 0 );

  --resource.nUsers;

  log.print( "Unloading sound '%s' ...", library.sounds[id].name.cstr() );
  alDeleteBuffers( 1, &resource.id );

  OZ_AL_CHECK_ERROR();

  log.printEnd( " OK" );
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

void Context::drawImago( const Object* obj, const Imago* parent, int mask )
{
  hard_assert( obj->flags & Object::IMAGO_BIT );

  Imago* const* value = imagines.find( obj->index );

  if( value == null ) {
    const Imago::CreateFunc& createFunc = imagoClasses[obj->clazz->imagoType];

    value = imagines.add( obj->index, createFunc( obj ) );
  }

  Imago* imago = *value;

  imago->flags |= Imago::UPDATED_BIT;
  imago->draw( parent, mask );
}

void Context::playAudio( const Object* obj, const Audio* parent )
{
  hard_assert( obj->flags & Object::AUDIO_BIT );

  Audio* const* value = audios.find( obj->index );

  if( value == null ) {
    const Audio::CreateFunc& createFunc = audioClasses[obj->clazz->audioType];

    value = audios.add( obj->index, createFunc( obj ) );
  }

  Audio* audio = *value;

  audio->flags |= Audio::UPDATED_BIT;
  audio->play( parent );
}

void Context::updateLoad()
{
  maxImagines           = max( maxImagines,           imagines.length() );
  maxAudios             = max( maxAudios,             audios.length() );
  maxSources            = max( maxSources,            Source::pool.length() );
  maxBSPSources         = max( maxBSPSources,         bspSources.length() );
  maxObjSources         = max( maxObjSources,         objSources.length() );

  maxSMMImagines        = max( maxSMMImagines,        SMMImago::pool.length() );
  maxSMMVehicleImagines = max( maxSMMVehicleImagines, SMMVehicleImago::pool.length() );
  maxExplosionImagines  = max( maxExplosionImagines,  ExplosionImago::pool.length() );
  maxMD2Imagines        = max( maxMD2Imagines,        MD2Imago::pool.length() );
  maxMD2WeaponImagines  = max( maxMD2WeaponImagines,  MD2WeaponImago::pool.length() );
  maxMD3Imagines        = max( maxMD3Imagines,        MD3Imago::pool.length() );

  maxBasicAudios        = max( maxBasicAudios,        BasicAudio::pool.length() );
  maxBotAudios          = max( maxBotAudios,          BotAudio::pool.length() );
  maxVehicleAudios      = max( maxVehicleAudios,      VehicleAudio::pool.length() );
}

void Context::load()
{
  maxImagines           = 0;
  maxAudios             = 0;
  maxSources            = 0;
  maxBSPSources         = 0;
  maxObjSources         = 0;

  maxSMMImagines        = 0;
  maxSMMVehicleImagines = 0;
  maxExplosionImagines  = 0;
  maxMD2Imagines        = 0;
  maxMD2WeaponImagines  = 0;
  maxMD3Imagines        = 0;

  maxBasicAudios        = 0;
  maxBotAudios          = 0;
  maxVehicleAudios      = 0;
}

void Context::unload()
{
  log.println( "Unloading Context {" );
  log.indent();

  log.println( "Peak context instances {" );
  log.indent();
  log.println( "%6d  imago objects",                maxImagines );
  log.println( "%6d  audio objects",                maxAudios );
  log.println( "%6d  non-continuous sources",       maxSources );
  log.println( "%6d  structure continuous sources", maxBSPSources );
  log.println( "%6d  object continuous sources",    maxObjSources );
  log.println( "%6d  SMM imagines",                 maxSMMImagines );
  log.println( "%6d  SMMVehicle imagines",          maxSMMVehicleImagines );
  log.println( "%6d  Explosion imagines",           maxExplosionImagines );
  log.println( "%6d  MD2 imagines",                 maxMD2Imagines );
  log.println( "%6d  MD2Weapon imagines",           maxMD2WeaponImagines );
  log.println( "%6d  MD3 imagines",                 maxMD3Imagines );
  log.println( "%6d  Basic audios",                 maxBasicAudios );
  log.println( "%6d  Bot audios",                   maxBotAudios );
  log.println( "%6d  Vehicle audios",               maxVehicleAudios );
  log.unindent();
  log.println( "}" );

  imagines.free();
  imagines.dealloc();
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
    if( sounds[i].nUsers == 0 ) {
      freeSound( i );
    }
    hard_assert( sounds[i].nUsers == -1 );
  }

  hard_assert( glGetError() == AL_NO_ERROR );
  OZ_AL_CHECK_ERROR();

  Source::pool.free();

  SMMImago::pool.free();
  SMMVehicleImago::pool.free();
  ExplosionImago::pool.free();
  MD2Imago::pool.free();
  MD2WeaponImago::pool.free();
  MD3Imago::pool.free();

  BasicAudio::pool.free();
  BotAudio::pool.free();
  VehicleAudio::pool.free();

  log.unindent();
  log.println( "}" );
}

void Context::init()
{
  log.print( "Initialising Context ..." );

  if( library.nImagoClasses != 0 ) {
    imagoClasses = new Imago::CreateFunc[library.nImagoClasses];
  }
  if( library.nAudioClasses != 0 ) {
    audioClasses = new Audio::CreateFunc[library.nAudioClasses];
  }

  OZ_REGISTER_IMAGOCLASS( SMM );
  OZ_REGISTER_IMAGOCLASS( SMMVehicle );
  OZ_REGISTER_IMAGOCLASS( Explosion );
  OZ_REGISTER_IMAGOCLASS( MD2 );
  OZ_REGISTER_IMAGOCLASS( MD2Weapon );
  OZ_REGISTER_IMAGOCLASS( MD3 );

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

  textures = new Resource<uint>[ library.textures.length() ];
  sounds   = new Resource<uint>[ library.sounds.length() ];
  bsps     = new Resource<BSP*>[ library.bsps.length() ];
  smms     = new Resource<SMM*>[ library.models.length() ];
  md2s     = new Resource<MD2*>[ library.models.length() ];
  md3s     = new Resource<MD3*>[ library.models.length() ];

  for( int i = 0; i < library.textures.length(); ++i ) {
    textures[i].nUsers = 0;
  }
  for( int i = 0; i < library.sounds.length(); ++i ) {
    sounds[i].nUsers = -1;
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

  log.printEnd( " OK" );
}

void Context::free()
{
  log.print( "Freeing Context ..." );

  delete[] imagoClasses;
  delete[] audioClasses;

  delete[] textures;
  delete[] sounds;
  delete[] bsps;
  delete[] smms;
  delete[] md2s;
  delete[] md3s;

  imagoClasses = null;
  audioClasses = null;

  textures     = null;
  sounds       = null;
  bsps         = null;
  smms         = null;
  md2s         = null;
  md3s         = null;

  log.printEnd( " OK" );
}

}
}
