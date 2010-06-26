/*
 *  OBJModel.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING file for details.
 */

#include "stable.hpp"

#include "client/OBJModel.hpp"

#include "client/Context.hpp"

namespace oz
{
namespace client
{

  Pool<OBJModel, 0, 256> OBJModel::pool;

  Model* OBJModel::create( const Object* obj )
  {
    OBJModel* model = new OBJModel();

    model->obj  = obj;
    model->list = context.loadOBJ( obj->type->modelName );
    return model;
  }

  OBJModel::~OBJModel()
  {
    context.releaseOBJ( obj->type->modelName );
  }

  void OBJModel::draw( const Model* )
  {
    glCallList( list );
  }

}
}
