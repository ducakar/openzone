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
 * @file client/BasicAudio.cc
 */

#include "stable.hh"

#include "client/BasicAudio.hh"

#include "client/Context.hh"

namespace oz
{
namespace client
{

Pool<BasicAudio, 2048> BasicAudio::pool;

BasicAudio::BasicAudio( const Object* obj ) :
  Audio( obj )
{
  aSet( recent, 0, ObjectClass::MAX_SOUNDS );
}

Audio* BasicAudio::create( const Object* obj )
{
  return new BasicAudio( obj );
}

void BasicAudio::play( const Audio* parent )
{
  flags |= UPDATED_BIT;

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
      playSound( sounds[event->id], event->intensity, obj, parent == null ? obj : parent->obj );
    }
  }

  // friction
  if( parent == null && ( obj->flags & Object::DYNAMIC_BIT ) &&
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
}

}
}
