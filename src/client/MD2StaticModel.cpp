/*
 *  MD2StaticModel.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3.0. See COPYING for details.
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
    model->list = context.loadStaticMD2( obj->type->modelName );
    return model;
  }

  MD2StaticModel::~MD2StaticModel()
  {
    context.releaseStaticMD2( obj->type->modelName );
  }

  void MD2StaticModel::draw( const Model* )
  {
    glCallList( list );
  }

}
}
