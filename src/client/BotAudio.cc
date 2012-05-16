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

const float BotAudio::FOOTSTEP_DISTANCE_SQ = 32.0f*32.0f;

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

    if( event->id >= 0 && sounds[event->id] != -1 && recent[event->id] == 0 ) {
      hard_assert( 0.0f <= event->intensity );

      recent[event->id] = RECENT_TICKS;

      if( !( bot->state & Bot::DEAD_BIT ) || event->id == Bot::EVENT_DEATH ) {
        playSound( sounds[event->id], event->intensity, obj, parent == null ? obj : parent->obj );
      }
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
  if( ( bot->state & Bot::MOVING_BIT ) && ( bot->p - camera.p ).sqL() < FOOTSTEP_DISTANCE_SQ ) {
    if( bot->flags & Object::FRICTING_BIT ) {
      recent[Object::EVENT_FRICTING] = RECENT_TICKS;
    }

    int currStep = int( 2.0f * bot->step ) % 2;

    if( currStep != prevStep ) {
      if( bot->state & Bot::SWIMMING_BIT ) {
        if( !( bot->state & Bot::SUBMERGED_BIT ) && sounds[Bot::EVENT_SWIM] != -1 ) {
          playSound( sounds[Bot::EVENT_SWIM], 1.0f, bot, bot );
        }
      }
      else if( recent[Object::EVENT_FRICTING] != 0 ) {
        if( bot->depth != 0.0f ) {
          if( sounds[Bot::EVENT_WATERSTEP] != -1 ) {
            playSound( sounds[Bot::EVENT_WATERSTEP], 1.0f, bot, bot );
          }
        }
        else {
          if( sounds[Bot::EVENT_STEP] != -1 ) {
            playSound( sounds[Bot::EVENT_STEP], 1.0f, bot, bot );
          }
        }
      }
    }

    prevStep = currStep;
  }
  else {
    prevStep = 0;
  }
}

}
}
