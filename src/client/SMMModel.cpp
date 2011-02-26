/*
 *  OBJModel.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "client/SMMModel.hpp"

#include "client/Context.hpp"

namespace oz
{
namespace client
{

  Pool<SMMModel, 2048> SMMModel::pool;

  Model* SMMModel::create( const Object* obj )
  {
    SMMModel* model = new SMMModel();

    model->obj   = obj;
    model->clazz = obj->clazz;
    model->smm   = context.requestSMM( obj->clazz->modelIndex );
    return model;
  }

  SMMModel::~SMMModel()
  {
    context.releaseSMM( clazz->modelIndex );
  }

  void SMMModel::draw( const Model* )
  {
    if( smm->isLoaded ) {
      smm->draw();
    }
  }

}
}
