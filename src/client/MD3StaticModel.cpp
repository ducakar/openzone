/*
 *  MD3StaticModel.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3.0. See COPYING for details.
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
    model->list = context.loadStaticMD3( obj->type->modelName );
    return model;
  }

  MD3StaticModel::~MD3StaticModel()
  {
    context.releaseStaticMD3( obj->type->modelName );
  }

  void MD3StaticModel::draw( const Model* )
  {
    glCallList( list );
  }

}
}
