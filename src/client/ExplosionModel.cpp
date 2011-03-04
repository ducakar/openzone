/*
 *  ExplosionModel.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "client/ExplosionModel.hpp"

#include "matrix/Timer.hpp"
#include "client/Context.hpp"
#include "client/Colours.hpp"

#include <GL/glu.h>

namespace oz
{
namespace client
{

  Pool<ExplosionModel, 256> ExplosionModel::pool;

  Model* ExplosionModel::create( const Object* obj )
  {
    ExplosionModel* model = new ExplosionModel();

    model->obj = obj;
    model->texId = context.loadTexture( "textures/oz/explosion.jpg" );
    model->quadric = gluNewQuadric();
    model->startMillis = timer.millis;

    gluQuadricTexture( model->quadric, GL_TRUE );

    return model;
  }

  ExplosionModel::~ExplosionModel()
  {
    gluDeleteQuadric( quadric );
    glDeleteTextures( 1, &texId );
  }

  void ExplosionModel::draw( const Model* )
  {
    float millis = float( timer.millis - startMillis );
    float radius = millis * obj->dim.z * 0.006f;
    float alpha  = 1.0f - 0.001f * millis;
    float colour[] = { 1.0f, 1.0f, 1.0f, alpha*alpha };

    glEnable( GL_BLEND );
    glDisable( GL_CULL_FACE );

    glBindTexture( texId );
    gluSphere( quadric, radius, 32, 32 );

    glEnable( GL_CULL_FACE );
    glDisable( GL_BLEND );
  }

}
}
