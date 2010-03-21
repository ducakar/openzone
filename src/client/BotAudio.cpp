/*
 *  BotAudio.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
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

  Pool<BotAudio> BotAudio::pool;

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
      playContSound( samples[SND_FRICTING], dv, uint( &*bot ), obj );
    }

    // events
    foreach( event, bot->events.citer() ) {
      assert( event->id < ObjectClass::AUDIO_SAMPLES );

      if( event->id >= 0 && samples[event->id] != -1 ) {
        assert( 0.0f <= event->intensity );

        playSound( samples[event->id], event->intensity, obj );
      }
    }

    // inventory items' events
    foreach( item, bot->items.citer() ) {
      if( world.objects[*item] != null ) {
        sound.playAudio( world.objects[*item], this );
      }
    }
  }

}
}
