/*
 *  MD3StaticModel.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "client/MD3StaticModel.hpp"

#include "client/Context.hpp"

namespace oz
{
namespace client
{

  Pool<MD3StaticModel, 256> MD3StaticModel::pool;

  Model* MD3StaticModel::create( const Object* obj )
  {
    MD3StaticModel* model = new MD3StaticModel();

    model->obj  = obj;
    model->list = context.loadStaticMD3( obj->clazz->modelName );
    return model;
  }

  MD3StaticModel::~MD3StaticModel()
  {
    context.releaseStaticMD3( obj->clazz->modelName );
  }

  void MD3StaticModel::draw( const Model* )
  {
    glCallList( list );
  }

}
}
