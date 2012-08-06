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
 * @file client/VehicleAudio.cc
 */

#include "stable.hh"

#include "client/VehicleAudio.hh"

#include "client/Camera.hh"
#include "client/Context.hh"

namespace oz
{
namespace client
{

Pool<VehicleAudio, 256> VehicleAudio::pool;

Audio* VehicleAudio::create( const Object* obj )
{
  hard_assert( obj->flags & Object::VEHICLE_BIT );

  return new VehicleAudio( obj );
}

void VehicleAudio::play( const Audio* parent )
{
  flags |= UPDATED_BIT;

  const Vehicle*      vehicle = static_cast<const Vehicle*>( obj );
  const VehicleClass* clazz   = static_cast<const VehicleClass*>( this->clazz );
  const auto&         sounds  = obj->clazz->audioSounds;

  // engine sound
  if( ( vehicle->pilot >= 0 ) && sounds[Vehicle::EVENT_ENGINE] >= 0 ) {
    float pitch = clazz->enginePitchBias + min( vehicle->momentum.sqN() * clazz->enginePitchRatio,
                                                clazz->enginePitchLimit );

    playEngineSound( sounds[Vehicle::EVENT_ENGINE], 1.0f, pitch );
  }

  // events
  for( const Object::Event* event = obj->events.first(); event != null; event = event->next[0] ) {
    hard_assert( event->id < ObjectClass::MAX_SOUNDS );

    if( event->id >= 0 && sounds[event->id] >= 0 ) {
      hard_assert( 0.0f <= event->intensity );

      playSound( sounds[event->id], event->intensity, parent == null ? obj : parent->obj );
    }
  }

  // inventory items' events
  for( int i = 0; i < obj->items.length(); ++i ) {
    const Object* item = orbis.objects[ obj->items[i] ];

    if( item != null && ( item->flags & Object::AUDIO_BIT ) ) {
      context.playAudio( item, parent == null ? this : parent );
    }
  }

  // pilot
  if( vehicle->pilot >= 0 && camera.bot == vehicle->pilot ) {
    const Bot* bot = static_cast<const Bot*>( orbis.objects[vehicle->pilot] );

    hard_assert( bot->flags & Object::BOT_BIT );

    if( bot != null && ( bot->flags & Object::AUDIO_BIT ) ) {
      context.playAudio( bot, parent == null ? this : parent );
    }
  }
}

}
}
