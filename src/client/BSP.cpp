/*
 *  BSP.cpp
 *
 *  BSP level rendering class
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "client/BSP.hpp"

#include "client/Context.hpp"
#include "client/OpenGL.hpp"
#include "client/OpenAL.hpp"

namespace oz
{
namespace client
{

void BSP::playSound( const Struct::Entity* entity, int sample ) const
{
  hard_assert( uint( sample ) < uint( library.sounds.length() ) );

  Bounds bounds = *entity->model;
  Point3 localPos = bounds.mins + 0.5f * ( bounds.maxs - bounds.mins );
  Point3 p = entity->str->toAbsoluteCS( localPos + entity->offset );

  uint srcId;

  alGenSources( 1, &srcId );
  if( alGetError() != AL_NO_ERROR ) {
    log.println( "AL: Too many sources" );
    return;
  }

  alSourcei( srcId, AL_BUFFER, int( context.sounds[sample].id ) );
  alSourcef( srcId, AL_REFERENCE_DISTANCE, Audio::REFERENCE_DISTANCE );
  alSourcef( srcId, AL_ROLLOFF_FACTOR, Audio::ROLLOFF_FACTOR );

  alSourcefv( srcId, AL_POSITION, p );
  alSourcef( srcId, AL_GAIN, 1.0f );
  alSourcePlay( srcId );

  context.addSource( srcId, sample );

  OZ_AL_CHECK_ERROR();
}

void BSP::playContSound( const Struct::Entity* entity, int sample ) const
{
  hard_assert( uint( sample ) < uint( library.sounds.length() ) );

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

    alSourcei( srcId, AL_BUFFER, int( context.sounds[sample].id ) );
    alSourcei( srcId, AL_LOOPING, AL_TRUE );
    alSourcef( srcId, AL_ROLLOFF_FACTOR, 0.25f );

    alSourcefv( srcId, AL_POSITION, p );
    alSourcef( srcId, AL_GAIN, 1.0f );
    alSourcePlay( srcId );

    context.addBSPSource( srcId, sample, key );
  }
  else {
    alSourcefv( contSource->id, AL_POSITION, p );

    contSource->isUpdated = true;
  }

  OZ_AL_CHECK_ERROR();
}

BSP::BSP( int id_ ) : id( id_ ), flags( 0 ), bsp( orbis.bsps[id_] ), isLoaded( false )
{
  const BSPClass& clazz = library.bspClasses[id];

  for( int i = 0; i < clazz.sounds.length(); ++i ) {
    context.requestSound( clazz.sounds[i] );
  }
}

BSP::~BSP()
{
  log.println( "Unloading BSP model '%s' {", library.bsps[id].name.cstr() );
  log.indent();

  if( isLoaded ) {
    mesh.unload();
  }

  const BSPClass& clazz = library.bspClasses[id];

  for( int i = 0; i < clazz.sounds.length(); ++i ) {
    context.releaseSound( clazz.sounds[i] );
  }

  log.unindent();
  log.println( "}" );
}

void BSP::draw( const Struct* str, int mask ) const
{
  mask &= flags;

  if( mask == 0 ) {
    return;
  }

  mesh.bind();
  tf.apply();

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
    const Struct::Entity& entity = str->entities[i];

    if( entity.state == Struct::Entity::OPENING ) {
      if( entity.ratio == 0.0f && entity.model->openSample != -1 ) {
        playSound( &entity, entity.model->openSample );
      }
      if( entity.model->frictSample != -1 ) {
        playContSound( &entity, entity.model->frictSample );
      }
    }
    else if( entity.state == Struct::Entity::CLOSING ) {
      if( entity.ratio == 1.0f && entity.model->closeSample != -1 ) {
        playSound( &entity, entity.model->closeSample );
      }
      if( entity.model->frictSample != -1 ) {
        playContSound( &entity, entity.model->frictSample );
      }
    }
  }
}

void BSP::load()
{
  const String& name = library.bsps[id].name;

  log.println( "Loading BSP model '%s' {", name.cstr() );
  log.indent();

  File file( "bsp/" + name + ".ozcBSP" );
  if( !file.map() ) {
    throw Exception( "BSP file mmap failed" );
  }

  InputStream istream = file.inputStream();

  flags = istream.readInt();

  mesh.load( &istream, GL_STATIC_DRAW );

  file.unmap();

  log.unindent();
  log.println( "}" );

  isLoaded = true;
}

}
}
