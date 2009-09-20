/*
 *  MD2StaticModel.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#include "precompiled.h"

#include "MD2StaticModel.h"

#include "Context.h"

namespace oz
{
namespace client
{

  Model *MD2StaticModel::create( const Object *obj )
  {
    MD2StaticModel *model = new MD2StaticModel();

    model->obj  = obj;
    model->list = context.loadMD2StaticModel( obj->type->modelName );
    return model;
  }

  MD2StaticModel::~MD2StaticModel()
  {
    context.releaseMD2StaticModel( obj->type->modelName );
  }

  void MD2StaticModel::draw()
  {
    glCallList( list );
  }

}
}
