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

  Model *MD3StaticModel::create( const Object *object )
  {
    MD3StaticModel *model = new MD3StaticModel();

    model->object = object;
    model->list   = context.loadMD3StaticModel( object->type->modelPath );
    return model;
  }

  MD3StaticModel::~MD3StaticModel()
  {
    context.releaseMD3StaticModel( object->type->modelPath );
  }

  void MD3StaticModel::draw()
  {
    glCallList( list );
  }

}
}
