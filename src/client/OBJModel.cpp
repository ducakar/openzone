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

  Model *OBJModel::create( const Object *object )
  {
    OBJModel *model = new OBJModel();

    model->object = object;
    model->name   = object->type->modelName;
    model->list   = context.loadOBJModel( object->type->modelName );
    return model;
  }

  OBJModel::~OBJModel()
  {
    context.releaseOBJModel( name );
  }

  void OBJModel::draw()
  {
    glCallList( list );
  }

}
}
