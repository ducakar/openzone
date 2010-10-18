/*
 *  MD2StaticModel.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "client/MD2StaticModel.hpp"

#include "client/Context.hpp"

namespace oz
{
namespace client
{

  Pool<MD2StaticModel, 0, 256> MD2StaticModel::pool;

  Model* MD2StaticModel::create( const Object* obj )
  {
    MD2StaticModel* model = new MD2StaticModel();

    model->obj  = obj;
    model->list = context.loadStaticMD2( obj->clazz->modelName );
    return model;
  }

  MD2StaticModel::~MD2StaticModel()
  {
    context.releaseStaticMD2( obj->clazz->modelName );
  }

  void MD2StaticModel::draw( const Model* )
  {
    glCallList( list );
  }

}
}
