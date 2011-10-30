/*
 *  VehicleAudio.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "client/VehicleAudio.hpp"

#include "matrix/Orbis.hpp"
#include "matrix/VehicleClass.hpp"

#include "client/Context.hpp"

namespace oz
{
namespace client
{

Pool<VehicleAudio> VehicleAudio::pool;

Audio* VehicleAudio::create( const Object* obj )
{
  hard_assert( obj->flags & Object::VEHICLE_BIT );

  return new VehicleAudio( obj );
}

void VehicleAudio::play( const Audio* parent )
{
  const Vehicle* vehicle = static_cast<const Vehicle*>( obj );
  const VehicleClass* clazz = static_cast<const VehicleClass*>( this->clazz );
  const int ( &samples )[ObjectClass::AUDIO_SAMPLES] = obj->clazz->audioSamples;

  // engine sound
  if( vehicle->pilot != -1 && samples[Vehicle::EVENT_ENGINE] != -1 ) {
    float pitch = clazz->enginePitchBias + min( vehicle->momentum.sqL() * clazz->enginePitchRatio,
                                                clazz->enginePitchLimit );

    playEngineSound( samples[Vehicle::EVENT_ENGINE], 1.0f, pitch, obj );
  }

  // events
  for( auto event : vehicle->events.citer() ) {
    hard_assert( event->id < ObjectClass::AUDIO_SAMPLES );

    if( event->id >= 0 && samples[event->id] != -1 ) {
      hard_assert( 0.0f <= event->intensity );

      playSound( samples[event->id], event->intensity, obj, parent == null ? obj : parent->obj );
    }
  }

  // pilot
  if( vehicle->pilot != -1 ) {
    const Bot* bot = static_cast<const Bot*>( orbis.objects[vehicle->pilot] );

    hard_assert( bot->flags & Object::BOT_BIT );

    if( bot != null && ( bot->flags & Object::AUDIO_BIT ) ) {
      context.playAudio( bot, parent == null ? this : parent );
    }
  }
}

}
}
