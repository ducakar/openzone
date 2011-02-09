/*
 *  Audio.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "client/Audio.hpp"

#include "client/Camera.hpp"
#include "client/Context.hpp"

#include <AL/alut.h>

namespace oz
{
namespace client
{

  const float Audio::REFERENCE_DISTANCE = 4.0f;

  void Audio::playSound( int sample, float volume, const Object* obj ) const
  {
    hard_assert( uint( sample ) < uint( translator.sounds.length() ) );
    hard_assert( alGetError() == AL_NO_ERROR );

    uint srcId;

    alGenSources( 1, &srcId );
    if( alGetError() != AL_NO_ERROR ) {
      log.println( "AL: Too many sources" );
      return;
    }

    alSourcei( srcId, AL_BUFFER, context.sounds[sample].id );
    alSourcef( srcId, AL_REFERENCE_DISTANCE, REFERENCE_DISTANCE );

    // If the object moves since source starts playing and source stands still, it's usually
    // not noticeable for short-time source. After all, sound source many times does't move
    // together with the object in many cases (e.g. the sound when an objects hits something).
    //
    // However, when the sound is generated by the player (e.g. cries, talk) it is often annoying
    // if the sound source doesn't move with the player. That's why we position such sounds
    // at the origin of the coordinate system relative to player.
    if( obj->index == camera.bot ) {
      alSourcei( srcId, AL_SOURCE_RELATIVE, AL_TRUE );
      alSourcefv( srcId, AL_POSITION, Vec3::ZERO );
    }
    else {
      alSourcefv( srcId, AL_POSITION, obj->p );
    }
    alSourcef( srcId, AL_GAIN, volume );
    alSourcePlay( srcId );

    hard_assert( alGetError() == AL_NO_ERROR );

    context.sources.add( new Context::Source( srcId ) );
  }

  void Audio::playContSound( int sample, float volume, const Object* obj ) const
  {
    hard_assert( uint( sample ) < uint( translator.sounds.length() ) );
    hard_assert( alGetError() == AL_NO_ERROR );

    Context::ContSource* contSource = context.contSources.find( obj->index );

    if( contSource != null ) {
      alSourcef( contSource->source, AL_GAIN, volume );
      alSourcefv( contSource->source, AL_POSITION, obj->p );

      contSource->isUpdated = true;
    }
    else {
      uint srcId;

      alGenSources( 1, &srcId );
      if( alGetError() != AL_NO_ERROR ) {
        log.println( "AL: Too many sources" );
        return;
      }

      alSourcei( srcId, AL_BUFFER, context.sounds[sample].id );
      alSourcei( srcId, AL_LOOPING, AL_TRUE );
      alSourcef( srcId, AL_REFERENCE_DISTANCE, REFERENCE_DISTANCE );
      alSourcefv( srcId, AL_POSITION, obj->p );
      alSourcef( srcId, AL_GAIN, volume );
      alSourcePlay( srcId );

      context.contSources.add( obj->index, Context::ContSource( srcId ) );
    }

    hard_assert( alGetError() == AL_NO_ERROR );
  }

  Audio::Audio( const Object* obj_ ) : obj( obj_ ), flags( 0 ), clazz( obj_->clazz )
  {
    const int* samples = clazz->audioSamples;

    for( int i = 0; i < ObjectClass::AUDIO_SAMPLES; ++i ) {
      if( samples[i] != -1 ) {
        context.requestSound( samples[i] );
      }
    }
    hard_assert( alGetError() == AL_NO_ERROR );
  }

  Audio::~Audio()
  {
    const int* samples = clazz->audioSamples;

    for( int i = 0; i < ObjectClass::AUDIO_SAMPLES; ++i ) {
      if( samples[i] != -1 ) {
        context.releaseSound( samples[i] );
      }
    }
    hard_assert( alGetError() == AL_NO_ERROR );
  }

}
}
