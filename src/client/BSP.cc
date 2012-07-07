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
 * @file client/BSP.cc
 */

#include "stable.hh"

#include "client/BSP.hh"

#include "client/Context.hh"
#include "client/OpenGL.hh"
#include "client/OpenAL.hh"

namespace oz
{
namespace client
{

void BSP::playDemolish( const Struct* str, int sound ) const
{
  hard_assert( uint( sound ) < uint( library.sounds.length() ) );

  uint srcId;

  alGenSources( 1, &srcId );
  if( alGetError() != AL_NO_ERROR ) {
    Log::println( "AL: Too many sources" );
    return;
  }

  alSourcei( srcId, AL_BUFFER, int( context.sounds[sound].id ) );
  alSourcef( srcId, AL_REFERENCE_DISTANCE, Audio::REFERENCE_DISTANCE );

  alSourcefv( srcId, AL_POSITION, str->p );

  alSourcePlay( srcId );

  context.addSource( srcId, sound );

  OZ_AL_CHECK_ERROR();
}

void BSP::playSound( const Entity* entity, int sound ) const
{
  hard_assert( uint( sound ) < uint( library.sounds.length() ) );

  const Struct* str      = entity->str;
  Point         p        = str->toAbsoluteCS( entity->model->p() + entity->offset );
  Vec3          velocity = str->toAbsoluteCS( entity->velocity );

  uint srcId;

  alGenSources( 1, &srcId );
  if( alGetError() != AL_NO_ERROR ) {
    Log::println( "AL: Too many sources" );
    return;
  }

  alSourcei( srcId, AL_BUFFER, int( context.sounds[sound].id ) );
  alSourcef( srcId, AL_REFERENCE_DISTANCE, Audio::REFERENCE_DISTANCE );

  alSourcefv( srcId, AL_POSITION, p );
  alSourcefv( srcId, AL_VELOCITY, velocity );

  alSourcePlay( srcId );

  context.addSource( srcId, sound );

  OZ_AL_CHECK_ERROR();
}

void BSP::playContSound( const Entity* entity, int sound ) const
{
  hard_assert( uint( sound ) < uint( library.sounds.length() ) );

  const Struct* str      = entity->str;
  int           key      = str->index * Struct::MAX_ENTITIES + int( entity - str->entities );
  Point         p        = str->toAbsoluteCS( entity->model->p() + entity->offset );
  Vec3          velocity = str->toAbsoluteCS( entity->velocity );

  Context::ContSource* contSource = context.contSources.find( key );

  if( contSource == null ) {
    uint srcId;

    alGenSources( 1, &srcId );
    if( alGetError() != AL_NO_ERROR ) {
      Log::println( "AL: Too many sources" );
      return;
    }

    alSourcei( srcId, AL_BUFFER, int( context.sounds[sound].id ) );
    alSourcef( srcId, AL_REFERENCE_DISTANCE, Audio::REFERENCE_DISTANCE );

    alSourcei( srcId, AL_LOOPING, AL_TRUE );
    alSourcefv( srcId, AL_POSITION, p );
    alSourcefv( srcId, AL_VELOCITY, velocity );
    alSourcePlay( srcId );

    context.addContSource( srcId, sound, key );
  }
  else {
    alSourcefv( contSource->id, AL_POSITION, p );

    contSource->isUpdated = true;
  }

  OZ_AL_CHECK_ERROR();
}

BSP::BSP( const matrix::BSP* bsp_ ) :
  bsp( bsp_ ), flags( 0 ), isPreloaded( false ), isLoaded( false )
{
  for( int i = 0; i < bsp->sounds.length(); ++i ) {
    context.requestSound( bsp->sounds[i] );
  }
}

BSP::~BSP()
{
  for( int i = 0; i < bsp->sounds.length(); ++i ) {
    context.releaseSound( bsp->sounds[i] );
  }

  if( isLoaded ) {
    mesh.unload();
  }
}

void BSP::preload()
{
  file.setPath( "bsp/" + bsp->name + ".ozcBSP" );

  if( !file.map() ) {
    throw Exception( "BSP file '%s' mmap failed", file.path().cstr() );
  }

  isPreloaded = true;
}

void BSP::load()
{
  InputStream istream = file.inputStream();

  flags = istream.readInt();

  waterFogColour = istream.readVec4();
  lavaFogColour = istream.readVec4();

  mesh.load( &istream, GL_STATIC_DRAW, file.path() );

  hard_assert( !istream.isAvailable() );

  file.setPath( "" );

  isLoaded = true;
}

void BSP::draw( const Struct* str )
{
  tf.model = Mat44::translation( str->p - Point::ORIGIN );
  tf.model.rotateZ( float( str->heading ) * Math::TAU / 4.0f );

  mesh.schedule( 0 );

  for( int i = 0; i < str->nEntities; ++i ) {
    tf.push();
    tf.model.translate( str->entities[i].offset );

    mesh.schedule( i + 1 );

    tf.pop();
  }
}

void BSP::play( const Struct* str ) const
{
  if( str->life <= 0.0f && str->demolishing == 0.0f ) {
    int demolishSound = str->bsp->demolishSound;

    if( demolishSound >= 0 ) {
      playDemolish( str, demolishSound );
    }
  }

  for( int i = 0; i < str->nEntities; ++i ) {
    const Entity& entity = str->entities[i];

    if( entity.state == Entity::OPENING ) {
      if( entity.time == 0.0f && entity.model->openSound >= 0 ) {
        playSound( &entity, entity.model->openSound );
      }
      if( entity.model->frictSound >= 0 ) {
        playContSound( &entity, entity.model->frictSound );
      }
    }
    else if( entity.state == Entity::CLOSING ) {
      if( entity.time == 0.0f && entity.model->closeSound >= 0 ) {
        playSound( &entity, entity.model->closeSound );
      }
      if( entity.model->frictSound >= 0 ) {
        playContSound( &entity, entity.model->frictSound );
      }
    }
  }
}

}
}
