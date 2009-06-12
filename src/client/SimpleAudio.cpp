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

    return audio;
  }

  void SimpleAudio::update()
  {
    // non-continous
    foreach( event, Audio::object->events.iterator() ) {
      playSoundEvent( event );
    }
    // continous
    foreach( effect, object->effects.iterator() ) {
      playSoundEffect( effect );
    }
  }

}
}
