/*
 *  BotAudio.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING for details.
 */

#include "precompiled.h"

#include "BotAudio.h"

#include "matrix/Physics.h"
#include "Camera.h"
#include "Sound.h"

namespace oz
{
namespace client
{

  Audio* BotAudio::create( const Object* obj )
  {
    assert( obj->flags & Object::BOT_BIT );

    return new BotAudio( obj );
  }

  void BotAudio::play( const Audio* )
  {
    const Bot* bot = static_cast<const Bot*>( obj );
    const int ( &samples )[ObjectClass::AUDIO_SAMPLES] = obj->type->audioSamples;

    // friction
    if( ( bot->flags & ( Object::DYNAMIC_BIT | Object::FRICTING_BIT | Object::ON_SLICK_BIT ) ) ==
        ( Object::DYNAMIC_BIT | Object::FRICTING_BIT ) &&
        samples[SND_FRICTING] != -1 && bot->depth == 0.0f )
    {
      float dv = Math::sqrt( bot->velocity.x*bot->velocity.x +
                             bot->velocity.y*bot->velocity.y );
      playContSound( samples[SND_FRICTING], dv, reinterpret_cast<uint>( &*bot ), obj );
    }

    // events
    foreach( event, bot->events.iterator() ) {
      assert( event->id < ObjectClass::AUDIO_SAMPLES );

      if( event->id >= 0 && samples[event->id] != -1 ) {
        assert( 0.0f <= event->intensity );

        playSound( samples[event->id], event->intensity, obj );
      }
    }

    // weapon events
    if( bot->weaponItem != -1 && world.objects[bot->weaponItem] != null ) {
      sound.playAudio( world.objects[bot->weaponItem], this );
    }
  }

}
}
