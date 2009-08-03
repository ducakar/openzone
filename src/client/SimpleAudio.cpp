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

  Audio *SimpleAudio::create( const Object *obj )
  {
    assert( obj->flags & Object::DYNAMIC_BIT );

    return new SimpleAudio( obj, obj->type );
  }

  void SimpleAudio::update()
  {
    DynObject *obj = (DynObject*) this->obj;
    int ( &samples )[ObjectClass::AUDIO_SAMPLES] = obj->type->audioSamples;

    // friction
    if( obj->flags & Object::FRICTING_BIT ) {
//       float dv2 = obj->velocity.x*obj->velocity.x + obj->velocity.y*obj->velocity.y;
      playContSound( samples[SND_FRICTING], 1.0f, (uint) &*obj );
    }

    // splash
    if( ( obj->flags & Object::IN_WATER_BIT ) && !( obj->flags & Object::IN_WATER_BIT ) ) {
      if( obj->velocity.z < -4.0f ) {
        playSound( samples[SND_SPLASH_HARD], /*obj->velocity.z / -8.0f*/ 1.0f );
      }
      else {
        playSound( samples[SND_SPLASH_SOFT], /*obj->velocity.z / -8.0f*/ 1.0f );
      }
    }

    // events
    foreach( event, obj->events.iterator() ) {
      if( event->id == Object::EVENT_HIT ) {
        if( event->intensity < -4.0f ) {
          playSound( samples[SND_HIT_HARD], /*event->intensity / -8.0f*/ 1.0f );
        }
        else {
          playSound( samples[SND_HIT_HARD], /*event->intensity / -8.0f*/ 1.0f );
        }
      }
      else {
        playSound( samples[event->id], event->intensity );
      }
    }
  }

}
}
