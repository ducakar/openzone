/*
 *  SMMVehicleImago.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
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
