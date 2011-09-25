/*
 *  ExplosionModel.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "client/ExplosionModel.hpp"

#include "matrix/Timer.hpp"
#include "client/Context.hpp"
#include "client/Colours.hpp"

#include "client/OpenGL.hpp"

namespace oz
{
namespace client
{

  int ExplosionModel::modelId;

  Pool<ExplosionModel> ExplosionModel::pool;

  Model* ExplosionModel::create( const Object* obj )
  {
    ExplosionModel* model = new ExplosionModel();

    modelId = library.modelIndex( "explosion" );

    model->obj = obj;
    model->smm = context.requestSMM( modelId );
    model->startMillis = timer.millis;

    return model;
  }

  ExplosionModel::~ExplosionModel()
  {
    context.releaseSMM( modelId );
  }

  void ExplosionModel::draw( const Model* )
  {
    if( !smm->isLoaded ) {
      return;
    }

    float millis = float( timer.millis - startMillis );
    float radius = millis * obj->dim.z * 0.004f;
    float alpha  = 1.0f - 0.002f * millis;

    glEnable( GL_BLEND );
    glDisable( GL_CULL_FACE );

    shader.colour = Vec4( 1.0f, 1.0f, 1.0f, alpha*alpha );

    tf.model.scale( Vec3( radius, radius, radius ) );
    tf.apply();

    smm->draw();

    shader.colour = Colours::WHITE;

    glEnable( GL_CULL_FACE );
    glDisable( GL_BLEND );
  }

}
}
