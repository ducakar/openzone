/*
 *  SimpleAudio.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#include "precompiled.h"

#include "SimpleAudio.h"

#include "Camera.h"
#include "SoundManager.h"

namespace oz
{
namespace client
{

  void SimpleAudio::update()
  {
    // non-continous
    foreach( event, Audio::obj->events.iterator() ) {
      ALuint srcId;

      alGenSources( 1, &srcId );
      alSourcei( srcId, AL_BUFFER, soundManager.buffers[ event->id ] );
      alSourcef( srcId, AL_REFERENCE_DISTANCE, 2.0f );

      // If the object moves since source starts playing and source stands still, it's usually
      // not noticable for short-time source. After all, sound source many times does't move
      // together with the object in many cases (e.g. the sound when an objects hits something).
      //
      // However, when the sound is generated by player (e.g. cries, talk) it is often annoying
      // if the sound source doesn't move with the player. That's why we position the sounds
      // generated by the player at the origin of the coordinate system relative to player.
      if( &*obj == camera.player ) {
        alSourcei( srcId, AL_SOURCE_RELATIVE, AL_TRUE );
        alSourcefv( srcId, AL_POSITION, Vec3::zero() );
      }
      else {
        alSourcefv( srcId, AL_POSITION, obj->p );
      }
      alSourcePlay( srcId );

      soundManager.addSource( srcId );
    }
    // continous
    foreach( effect, obj->effects.iterator() ) {
      if( soundManager.updateContSource( effect ) ) {
        alSourcefv( soundManager.getCachedContSourceId(), AL_POSITION, obj->p );
      }
      else {
        ALuint srcId;

        alGenSources( 1, &srcId );
        alSourcei( srcId, AL_BUFFER, soundManager.buffers[ effect->id ] );
        alSourcefv( srcId, AL_POSITION, obj->p );
        alSourcei( srcId, AL_LOOPING, AL_TRUE );
        alSourcePlay( srcId );

        soundManager.addContSource( effect, srcId );
      }
    }
  }

}
}