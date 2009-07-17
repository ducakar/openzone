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

  Audio *SimpleAudio::create( const Object *object )
  {
    SimpleAudio *audio = new SimpleAudio();

    audio->object = object;
    audio->requestSounds();

    return audio;
  }

  void SimpleAudio::update()
  {
    int ( &samples )[ObjectClass::AUDIO_SAMPLES] = object->type->audioSamples;

    if( ( object->flags & Object::HIT_BIT ) && samples[SND_HIT] >= 0 ) {
      playSoundEvent( samples[SND_HIT] );
    }
    // non-continous
    foreach( event, Audio::object->events.iterator() ) {
      playSoundEvent( samples[event->id] );
    }
    // continous
    foreach( effect, object->effects.iterator() ) {
      playSoundEffect( samples[effect->id], (uint) &*effect );
    }
  }

}
}
