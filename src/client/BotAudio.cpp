/*
 * OpenZone - Simple Cross-Platform FPS/RTS Game Engine
 * Copyright (C) 2002-2011  Davorin Učakar
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Davorin Učakar <davorin.ucakar@gmail.com>
 */

/**
 * @file client/BotAudio.cpp
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

Pool<BotAudio, 256> BotAudio::pool;

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
  const int ( &sounds )[ObjectClass::MAX_SOUNDS] = obj->clazz->audioSounds;

  int objFlags = oldFlags[0] | oldFlags[1] | obj->flags;
  oldFlags[timer.ticks % 2] = obj->flags;

  // friction
  if( parent == null &&
      ( objFlags & ( Object::DYNAMIC_BIT | Object::FRICTING_BIT | Object::ON_SLICK_BIT ) ) ==
      ( Object::DYNAMIC_BIT | Object::FRICTING_BIT ) && sounds[Object::EVENT_FRICTING] != -1 )
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

    playContSound( sounds[Object::EVENT_FRICTING], Math::sqrt( dvx*dvx + dvy*dvy ), obj, obj );
  }

  // events
  for( const Object::Event* event = obj->events.first(); event != null; event = event->next[0] ) {
    hard_assert( event->id < ObjectClass::MAX_SOUNDS );

    if( event->id >= 0 && sounds[event->id] != -1 ) {
      hard_assert( 0.0f <= event->intensity );

      if( event->id == Object::EVENT_DAMAGE && ( bot->state & Bot::DEAD_BIT ) ) {
        continue;
      }

      playSound( sounds[event->id], event->intensity, obj, parent == null ? obj : parent->obj );
    }
  }

  // inventory items' events
  for( int i = 0; i < obj->items.length(); ++i ) {
    const Object* item = orbis.objects[ obj->items[i] ];

    if( item != null && ( item->flags & Object::AUDIO_BIT ) ) {
      context.playAudio( item, parent == null ? this : parent );
    }
  }
}

}
}
