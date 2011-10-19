/*
 *  OBJModel.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "client/SMMModel.hpp"

#include "client/Context.hpp"

namespace oz
{
namespace client
{

  Pool<SMMModel, 1024> SMMModel::pool;

  Model* SMMModel::create( const Object* obj )
  {
    SMMModel* model = new SMMModel();

    model->obj   = obj;
    model->clazz = obj->clazz;
    model->smm   = context.requestSMM( obj->clazz->modelIndex );
    model->h     = obj->flags & Object::RANDOM_HEADING_BIT ?
        Math::mod( ( obj->p.x + obj->p.y + obj->p.z ) * Math::E, Math::TAU ) :
        0.0f;

    return model;
  }

  SMMModel::~SMMModel()
  {
    context.releaseSMM( clazz->modelIndex );
  }

  void SMMModel::draw( const Model*, int mask )
  {
    if( !smm->isLoaded ) {
      return;
    }

    tf.model.rotateZ( h );

    smm->draw( mask );
  }

}
}
