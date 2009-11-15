/*
 *  OBJVehicleModel.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3.0. See COPYING for details.
 */

#include "precompiled.h"

#include "OBJVehicleModel.h"

#include "matrix/Vehicle.h"
#include "Context.h"

namespace oz
{
namespace client
{

  Model *OBJVehicleModel::create( const Object *obj )
  {
    assert( obj->flags & Object::VEHICLE_BIT );

    OBJVehicleModel *model = new OBJVehicleModel();

    model->obj  = obj;
    model->list = context.loadOBJModel( obj->type->modelName );
    return model;
  }

  OBJVehicleModel::~OBJVehicleModel()
  {
    context.releaseOBJModel( obj->type->modelName );
  }

  void OBJVehicleModel::draw()
  {
    const Vehicle *veh = static_cast<const Vehicle*>( obj );

    glMultMatrixf( veh->rot.rotMat44() );
    glCallList( list );
  }

}
}
