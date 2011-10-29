/*
 *  BotAudio.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "client/BotAudio.hpp"

#include "matrix/Orbis.hpp"
#include "matrix/Bot.hpp"

#include "client/Context.hpp"

namespace oz
{
namespace client
{

  Pool<BotAudio> BotAudio::pool;

  BotAudio::BotAudio( const Object* obj ) : BasicAudio( obj )
  {}

  Audio* BotAudio::create( const Object* obj )
  {
    hard_assert( obj->flags & Object::BOT_BIT );

    return new BotAudio( obj );
  }

  void BotAudio::play( const Audio* parent )
  {
    const Bot* bot = static_cast<const Bot*>( obj );
    const int ( &samples )[ObjectClass::AUDIO_SAMPLES] = obj->clazz->audioSamples;

    int objFlags = oldFlags[0] | oldFlags[1] | obj->flags;
    oldFlags[timer.ticks % 2] = obj->flags;

    // friction
    if( parent == null &&
        ( objFlags & ( Object::DYNAMIC_BIT | Object::FRICTING_BIT | Object::ON_SLICK_BIT ) ) ==
        ( Object::DYNAMIC_BIT | Object::FRICTING_BIT ) && samples[Object::EVENT_FRICTING] != -1 )
    {
      float dvx = bot->velocity.x;
      float dvy = bot->velocity.y;

      if( bot->lower != -1 ) {
        const Dynamic* sDyn = static_cast<const Dynamic*>( orbis.objects[bot->lower] );

        if( sDyn != null ) {
          dvx -= sDyn->velocity.x;
          dvy -= sDyn->velocity.y;
        }
      }

      playContSound( samples[Object::EVENT_FRICTING], Math::sqrt( dvx*dvx + dvy*dvy ), obj, obj );
    }

    // events
    for( auto event : bot->events.citer() ) {
      hard_assert( event->id < ObjectClass::AUDIO_SAMPLES );

      if( event->id >= 0 && samples[event->id] != -1 ) {
        hard_assert( 0.0f <= event->intensity );

        if( event->id == Object::EVENT_DAMAGE && ( bot->state & Bot::DEAD_BIT ) ) {
          continue;
        }

        playSound( samples[event->id], event->intensity, obj, parent == null ? obj : parent->obj );
      }
    }

    // inventory items' events
    for( auto item : bot->items.citer() ) {
      const Object* obj = orbis.objects[*item];

      if( obj != null && ( obj->flags & Object::AUDIO_BIT ) ) {
        context.playAudio( obj, parent == null ? this : parent );
      }
    }
  }

}
}
