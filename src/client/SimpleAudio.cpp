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
      playSoundEvent( event );
    }
    // continous
    foreach( effect, obj->effects.iterator() ) {
      playSoundEffect( effect );
    }
  }

}
}
