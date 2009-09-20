/*
 *  OBJModel.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#include "precompiled.h"

#include "OBJModel.h"

#include "Context.h"

namespace oz
{
namespace client
{

  Model *OBJModel::create( const Object *obj )
  {
    OBJModel *model = new OBJModel();

    model->obj  = obj;
    model->list = context.loadOBJModel( obj->type->modelName );
    return model;
  }

  OBJModel::~OBJModel()
  {
    context.releaseOBJModel( obj->type->modelName );
  }

  void OBJModel::draw()
  {
    glCallList( list );
  }

}
}
