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

    model->list = context.loadOBJModel( object->type->modelPath );
    return model;
  }

  void OBJModel::draw()
  {
    glCallList( list );
  }

}
}
