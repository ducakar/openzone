/*
 *  MD3StaticModel.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#include "precompiled.h"

#include "MD3StaticModel.h"

#include "Context.h"

namespace oz
{
namespace client
{

  Model *MD3StaticModel::create( const Object *obj )
  {
    MD3StaticModel *model = new MD3StaticModel();

    model->obj  = obj;
    model->list = context.loadMD3StaticModel( obj->type->modelName );
    return model;
  }

  MD3StaticModel::~MD3StaticModel()
  {
    context.releaseMD3StaticModel( obj->type->modelName );
  }

  void MD3StaticModel::draw()
  {
    glCallList( list );
  }

}
}
