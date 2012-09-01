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
 * @file client/SMMVehicleImago.cc
 */

#include <stable.hh>
#include <client/SMMVehicleImago.hh>

#include <client/Context.hh>
#include <client/Camera.hh>

namespace oz
{
namespace client
{

Pool<SMMVehicleImago, 64> SMMVehicleImago::pool;

Imago* SMMVehicleImago::create( const Object* obj )
{
  hard_assert( obj->flags & Object::VEHICLE_BIT );

  SMMVehicleImago* imago = new SMMVehicleImago( obj );

  imago->smm = context.requestSMM( obj->clazz->imagoModel );

  return imago;
}

SMMVehicleImago::~SMMVehicleImago()
{
  context.releaseSMM( clazz->imagoModel );
}

void SMMVehicleImago::draw( const Imago* )
{
  if( !smm->isLoaded ) {
    return;
  }

  const Vehicle*      veh   = static_cast<const Vehicle*>( obj );
  const VehicleClass* clazz = static_cast<const VehicleClass*>( obj->clazz );

  if( shader.mode == Shader::SCENE ) {
    tf.model = Mat44::translation( obj->p - Point::ORIGIN ) * veh->rot;
    tf.model.rotateX( Math::TAU / -4.0f );

    if( veh->pilot >= 0 && ( veh->state & Vehicle::CREW_VISIBLE_BIT ) ) {
      const Bot* bot = static_cast<const Bot*>( orbis.objects[veh->pilot] );

      tf.push();
      tf.model.translate( clazz->pilotPos );

      context.drawImago( bot, this );

      tf.pop();
    }
  }

  smm->schedule( -1 );
}

}
}
