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

void BSP::playSound( const Entity* entity, int sound ) const
{
  hard_assert( uint( sound ) < uint( library.sounds.length() ) );

  Bounds bounds = *entity->model;
  Point3 localPos = bounds.mins + 0.5f * ( bounds.maxs - bounds.mins );
  Point3 p = entity->str->toAbsoluteCS( localPos + entity->offset );

  uint srcId;

  alGenSources( 1, &srcId );
  if( alGetError() != AL_NO_ERROR ) {
    log.println( "AL: Too many sources" );
    return;
  }

  alSourcei( srcId, AL_BUFFER, int( context.sounds[sound].id ) );
  alSourcef( srcId, AL_REFERENCE_DISTANCE, Audio::REFERENCE_DISTANCE );

  alSourcefv( srcId, AL_POSITION, p );
  alSourcef( srcId, AL_GAIN, 1.0f );
  alSourcePlay( srcId );

  context.addSource( srcId, sound );

  OZ_AL_CHECK_ERROR();
}

void BSP::playContSound( const Entity* entity, int sound ) const
{
  hard_assert( uint( sound ) < uint( library.sounds.length() ) );

  const Struct* str = entity->str;
  // we can have at most 100 models per BSP, so stride 128 should do
  int key = str->index * 128 + int( entity - str->entities );

  Bounds bounds = *entity->model;
  Point3 localPos = bounds.mins + 0.5f * ( bounds.maxs - bounds.mins );
  Point3 p = entity->str->toAbsoluteCS( localPos + entity->offset );

  Context::ContSource* contSource = context.bspSources.find( key );

  if( contSource == null ) {
    uint srcId;

    alGenSources( 1, &srcId );
    if( alGetError() != AL_NO_ERROR ) {
      log.println( "AL: Too many sources" );
      return;
    }

    Bounds bounds = *entity->model;
    Point3 p = bounds.mins + 0.5f * ( bounds.maxs - bounds.mins );

    p = entity->str->toAbsoluteCS( p + entity->offset );

    alSourcei( srcId, AL_BUFFER, int( context.sounds[sound].id ) );
    alSourcei( srcId, AL_LOOPING, AL_TRUE );
    alSourcef( srcId, AL_ROLLOFF_FACTOR, 0.25f );

    alSourcefv( srcId, AL_POSITION, p );
    alSourcef( srcId, AL_GAIN, 1.0f );
    alSourcePlay( srcId );

    context.addBSPSource( srcId, sound, key );
  }
  else {
    alSourcefv( contSource->id, AL_POSITION, p );

    contSource->isUpdated = true;
  }

  OZ_AL_CHECK_ERROR();
}

BSP::BSP( const matrix::BSP* bsp_ ) :
  bsp( bsp_ ), flags( 0 ), isLoaded( false )
{
  for( int i = 0; i < bsp->sounds.length(); ++i ) {
    context.requestSound( bsp->sounds[i] );
  }
}

BSP::~BSP()
{
  if( isLoaded ) {
    mesh.unload();
  }

  for( int i = 0; i < bsp->sounds.length(); ++i ) {
    context.releaseSound( bsp->sounds[i] );
  }
}

void BSP::draw( const Struct* str, int mask ) const
{
  mask &= flags;

  if( mask == 0 ) {
    return;
  }

  tf.model = Mat44::translation( str->p - Point3::ORIGIN );
  tf.model.rotateZ( float( str->heading ) * Math::TAU / 4.0f );
  tf.apply();

  mesh.bind();
  mesh.drawComponent( 0, mask );

  for( int i = 0; i < bsp->nModels; ++i ) {
    tf.push();
    tf.model.translate( str->entities[i].offset );
    tf.apply();

    mesh.drawComponent( i + 1, mask );

    tf.pop();
  }
}

void BSP::play( const Struct* str ) const
{
  for( int i = 0; i < bsp->nModels; ++i ) {
    const Entity& entity = str->entities[i];

    if( entity.state == Entity::OPENING ) {
      if( entity.ratio == 0.0f && entity.model->openSound != -1 ) {
        playSound( &entity, entity.model->openSound );
      }
      if( entity.model->frictSound != -1 ) {
        playContSound( &entity, entity.model->frictSound );
      }
    }
    else if( entity.state == Entity::CLOSING ) {
      if( entity.ratio == 1.0f && entity.model->closeSound != -1 ) {
        playSound( &entity, entity.model->closeSound );
      }
      if( entity.model->frictSound != -1 ) {
        playContSound( &entity, entity.model->frictSound );
      }
    }
  }
}

void BSP::load()
{
  PhysFile file( "bsp/" + bsp->name + ".ozcBSP" );
  if( !file.map() ) {
    throw Exception( "BSP file '%s' mmap failed", file.path().cstr() );
  }

  InputStream istream = file.inputStream();

  flags = istream.readInt();
  mesh.load( &istream, GL_STATIC_DRAW, file.path() );

  file.unmap();

  isLoaded = true;
}

}
}
