/*
 *  MD2StaticModel.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "client/MD2StaticModel.hpp"

#include "client/Context.hpp"
#include "client/MD2.hpp"

namespace oz
{
namespace client
{

  Pool<MD2StaticModel, 256> MD2StaticModel::pool;

  Model* MD2StaticModel::create( const Object* obj )
  {
    MD2StaticModel* model = new MD2StaticModel();

    model->obj   = obj;
    model->clazz = obj->clazz;
    model->md2   = context.loadStaticMD2( obj->clazz->modelName );
    return model;
  }

  MD2StaticModel::~MD2StaticModel()
  {
    context.releaseStaticMD2( clazz->modelName );
  }

  void MD2StaticModel::draw( const Model* )
  {
    if( !md2->isLoaded ) {
      return;
    }

    glCallList( md2->list );
  }

}
}
