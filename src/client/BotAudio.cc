/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2012 Davorin Učakar
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
 */

/**
 * @file client/BotAudio.cc
 */

#include "stable.hh"

#include "client/BotAudio.hh"

#include "client/Context.hh"
#include "client/Camera.hh"

namespace oz
{
namespace client
{

Pool<BotAudio, 256> BotAudio::pool;

BotAudio::BotAudio( const Object* obj ) :
  BasicAudio( obj ), prevStep( 0 )
{}

Audio* BotAudio::create( const Object* obj )
{
  hard_assert( obj->flags & Object::BOT_BIT );

  return new BotAudio( obj );
}

void BotAudio::play( const Audio* parent )
{
  flags |= UPDATED_BIT;

  const Bot* bot = static_cast<const Bot*>( obj );
  const int ( &sounds )[ObjectClass::MAX_SOUNDS] = obj->clazz->audioSounds;

  for( int i = 0; i < ObjectClass::MAX_SOUNDS; ++i ) {
    recent[i] = max( recent[i] - 1, 0 );
  }

  // events
  foreach( event, obj->events.citer() ) {
    hard_assert( event->id < ObjectClass::MAX_SOUNDS );

    if( event->id >= 0 && sounds[event->id] != -1 ) {
      hard_assert( 0.0f <= event->intensity );

      if( recent[event->id] == 0 ) {
        recent[event->id] = RECENT_TICKS;

        if( event->id != Object::EVENT_DAMAGE || !( bot->state & Bot::DEAD_BIT ) ) {
          playSound( sounds[event->id], event->intensity, obj, parent == null ? obj : parent->obj );
        }
      }
    }
  }

  // friction
  if( parent == null && ( obj->flags & Object::ON_SLICK_BIT ) &&
      sounds[Object::EVENT_FRICTING] != -1 )
  {
    const Dynamic* dyn = static_cast<const Dynamic*>( obj );

    if( dyn->flags & Object::FRICTING_BIT ) {
      recent[Object::EVENT_FRICTING] = RECENT_TICKS;
    }

    if( recent[Object::EVENT_FRICTING] != 0 ) {
      float dvx = dyn->velocity.x;
      float dvy = dyn->velocity.y;

      if( dyn->lower != -1 ) {
        const Dynamic* sDyn = static_cast<const Dynamic*>( orbis.objects[dyn->lower] );

        if( sDyn != null ) {
          dvx -= sDyn->velocity.x;
          dvy -= sDyn->velocity.y;
        }
      }

      playContSound( sounds[Object::EVENT_FRICTING], Math::sqrt( dvx*dvx + dvy*dvy ), dyn, dyn );
    }
  }

  // inventory items' events
  for( int i = 0; i < obj->items.length(); ++i ) {
    const Object* item = orbis.objects[ obj->items[i] ];

    if( item != null && ( item->flags & Object::AUDIO_BIT ) ) {
      context.playAudio( item, parent == null ? this : parent );
    }
  }

  // footsteps
  if( camera.bot == bot->index ) {
    if( bot->state & Bot::MOVING_BIT ) {
      int currStep = int( camera.botProxy.bobPhi / ( Math::TAU / 2.0f ) ) % 2;

      if( bot->flags & Object::FRICTING_BIT ) {
        recent[Object::EVENT_FRICTING] = RECENT_TICKS;
      }

      if( currStep != prevStep && sounds[Bot::EVENT_STEP] != -1 &&
          !( bot->state & Bot::SWIMMING_BIT ) && recent[Object::EVENT_FRICTING] != 0 )
      {
        playSound( sounds[Bot::EVENT_STEP], 1.0f, bot, bot );
      }

      prevStep = currStep;
    }
    else {
      prevStep = 0;
    }
  }
}

}
}
