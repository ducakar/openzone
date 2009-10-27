/*
 *  BasicAudio.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#include "precompiled.h"

#include "BasicAudio.h"

#include "Camera.h"
#include "Sound.h"

namespace oz
{
namespace client
{

  const float BasicAudio::MOMENTUM_TRESHOLD = -6.0f;
  const float BasicAudio::MOMENTUM_INTENSITY_RATIO = -1.0f / 8.0f;

  Audio *BasicAudio::create( const Object *obj )
  {
    assert( obj->flags & Object::DYNAMIC_BIT );

    return new BasicAudio( obj );
  }

  void BasicAudio::update()
  {
    DynObject *obj = (DynObject*) this->obj;
    int ( &samples )[ObjectClass::AUDIO_SAMPLES] = obj->type->audioSamples;

    // friction
    if( ( obj->flags & Object::FRICTING_BIT ) && ( ~obj->flags & Object::ON_SLICK_BIT ) &&
        samples[SND_FRICTING] >= 0 )
    {
      float dv = Math::sqrt( obj->velocity.x*obj->velocity.x + obj->velocity.y*obj->velocity.y );
      playContSound( samples[SND_FRICTING], dv / 1.0f, (uint) &*obj );
    }

    // splash
    if( obj->flags & ~obj->oldFlags & Object::IN_WATER_BIT ) {
      if( obj->velocity.z < MOMENTUM_TRESHOLD && samples[SND_SPLASH_HARD] >= 0 ) {
        playSound( samples[SND_SPLASH_HARD], obj->velocity.z * MOMENTUM_INTENSITY_RATIO );
      }
      else if( obj->velocity.z < -2.0f && samples[SND_SPLASH_SOFT] >= 0 ) {
        playSound( samples[SND_SPLASH_SOFT], obj->velocity.z * MOMENTUM_INTENSITY_RATIO );
      }
    }

    // events
    foreach( event, obj->events.iterator() ) {
      if( event->id == Object::EVENT_HIT ) {
        if( event->intensity < MOMENTUM_TRESHOLD && samples[SND_HIT_HARD] >= 0 ) {
          playSound( samples[SND_HIT_HARD], event->intensity * MOMENTUM_INTENSITY_RATIO );
        }
        else if( samples[SND_HIT_SOFT] >= 0 ) {
          playSound( samples[SND_HIT_SOFT], event->intensity * MOMENTUM_INTENSITY_RATIO );
        }
      }
      else if( samples[event->id] >= 0 ) {
        playSound( samples[event->id], event->intensity );
      }
    }
  }

}
}
