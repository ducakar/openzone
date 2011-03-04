/*
 *  SMMVehicleModel.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "client/SMMVehicleModel.hpp"

#include "matrix/Vehicle.hpp"
#include "matrix/VehicleClass.hpp"
#include "client/Context.hpp"
#include "client/Camera.hpp"

namespace oz
{
namespace client
{

  Pool<SMMVehicleModel, 256> SMMVehicleModel::pool;

  Model* SMMVehicleModel::create( const Object* obj )
  {
    hard_assert( obj->flags & Object::VEHICLE_BIT );

    SMMVehicleModel* model = new SMMVehicleModel();

    model->obj   = obj;
    model->clazz = obj->clazz;
    model->smm   = context.requestSMM( obj->clazz->modelIndex );
    return model;
  }

  SMMVehicleModel::~SMMVehicleModel()
  {
    context.releaseSMM( clazz->modelIndex );
  }

  void SMMVehicleModel::draw( const Model* )
  {
    if( !smm->isLoaded ) {
      return;
    }

    const Vehicle* veh = static_cast<const Vehicle*>( obj );
    const VehicleClass* clazz = static_cast<const VehicleClass*>( obj->clazz );

    glMultMatrixf( veh->rot.rotMat44() );

    for( int i = 0; i < Vehicle::CREW_MAX; ++i ) {
      int index = veh->crew[i];

      if( index != -1 && ( index != camera.bot || camera.isExternal ) ) {
        const Bot* bot = static_cast<const Bot*>( orbis.objects[veh->crew[i]] );

        glPushMatrix();
        glTranslatef( clazz->crewPos[i].x, clazz->crewPos[i].y, clazz->crewPos[i].z );
        glRotatef( -Math::deg( bot->h ), 0.0f, 0.0f, 1.0f );

        context.drawModel( bot, null );

        glPopMatrix();
      }
    }

    smm->draw();
  }

}
}
