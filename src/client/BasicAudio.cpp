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
 * Davorin Učakar
 * <davorin.ucakar@gmail.com>
 */

/**
 * @file client/BasicAudio.cpp
 */

#include "stable.hpp"

#include "client/BasicAudio.hpp"

#include "matrix/Orbis.hpp"

#include "client/Context.hpp"

namespace oz
{
namespace client
{

Pool<BasicAudio, 2048> BasicAudio::pool;

BasicAudio::BasicAudio( const Object* obj ) : Audio( obj )
{
  oldFlags[0] = 0;
  oldFlags[1] = 0;
}

Audio* BasicAudio::create( const Object* obj )
{
  return new BasicAudio( obj );
}

void BasicAudio::play( const Audio* parent )
{
  flags |= UPDATED_BIT;

  const Dynamic* dyn = static_cast<const Dynamic*>( obj );
  const int ( &sounds )[ObjectClass::MAX_SOUNDS] = obj->clazz->audioSounds;

  // prevent friction sound restarting when it suspends for a tick or two
  int objFlags = oldFlags[0] | oldFlags[1] | obj->flags;
  oldFlags[timer.ticks % 2] = obj->flags;

  // friction
  if( parent == null &&
      ( objFlags & ( Object::DYNAMIC_BIT | Object::FRICTING_BIT | Object::ON_SLICK_BIT ) ) ==
      ( Object::DYNAMIC_BIT | Object::FRICTING_BIT ) && sounds[Dynamic::EVENT_FRICTING] != -1 )
  {
    float dvx = dyn->velocity.x;
    float dvy = dyn->velocity.y;

    if( dyn->lower != -1 ) {
      const Dynamic* sDyn = static_cast<const Dynamic*>( orbis.objects[dyn->lower] );

      if( sDyn != null ) {
        dvx -= sDyn->velocity.x;
        dvy -= sDyn->velocity.y;
      }
    }

    playContSound( sounds[Dynamic::EVENT_FRICTING], Math::sqrt( dvx*dvx + dvy*dvy ), obj, obj );
  }

  // events
  for( const Object::Event* event = obj->events.first(); event != null; event = event->next[0] ) {
    hard_assert( event->id < ObjectClass::MAX_SOUNDS );

    if( event->id >= 0 && sounds[event->id] != -1 ) {
      hard_assert( 0.0f <= event->intensity );

      playSound( sounds[event->id], event->intensity, obj, parent == null ? obj : parent->obj );
    }
  }
}

}
}
