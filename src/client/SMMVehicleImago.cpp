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
 * @file client/SMMVehicleImago.cpp
 */

#include "stable.hpp"

#include "client/SMMVehicleImago.hpp"

#include "matrix/Vehicle.hpp"
#include "matrix/VehicleClass.hpp"
#include "client/Context.hpp"
#include "client/Camera.hpp"

namespace oz
{
namespace client
{

Pool<SMMVehicleImago, 64> SMMVehicleImago::pool;

Imago* SMMVehicleImago::create( const Object* obj )
{
  hard_assert( obj->flags & Object::VEHICLE_BIT );

  SMMVehicleImago* imago = new SMMVehicleImago();

  imago->obj   = obj;
  imago->clazz = obj->clazz;
  imago->smm   = context.requestSMM( obj->clazz->imagoModel );
  return imago;
}

SMMVehicleImago::~SMMVehicleImago()
{
  context.releaseSMM( clazz->imagoModel );
}

void SMMVehicleImago::draw( const Imago*, int mask )
{
  const Vehicle* veh = static_cast<const Vehicle*>( obj );
  const VehicleClass* clazz = static_cast<const VehicleClass*>( obj->clazz );

  if( !smm->isLoaded ) {
    return;
  }

  if( shader.activeProgram != shader.plain ) {
    tf.model.rotate( veh->rot );

    if( veh->state & Vehicle::CREW_VISIBLE_BIT ) {
      if( veh->pilot != -1 ) {
        const Bot* bot = static_cast<const Bot*>( orbis.objects[veh->pilot] );

        tf.push();
        tf.model.translate( clazz->pilotPos );

        context.drawImago( bot, this, mask );

        tf.pop();
      }
    }
  }

  smm->draw( mask );
}

}
}
