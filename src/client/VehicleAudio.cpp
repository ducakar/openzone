/*
 *  VehicleAudio.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "client/VehicleAudio.hpp"

#include "matrix/Vehicle.hpp"

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

  void VehicleAudio::play( const Audio* )
  {
    const Vehicle* vehicle = static_cast<const Vehicle*>( obj );
    const int ( &samples )[ObjectClass::AUDIO_SAMPLES] = obj->clazz->audioSamples;

    // events
    foreach( event, vehicle->events.citer() ) {
      hard_assert( event->id < ObjectClass::AUDIO_SAMPLES );

      if( event->id >= 0 && samples[event->id] != -1 ) {
        hard_assert( 0.0f <= event->intensity );

        playSound( samples[event->id], event->intensity, obj );
      }
    }
  }

}
}
