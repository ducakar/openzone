/*
 *  ExplosionImago.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "client/ExplosionImago.hpp"

#include "client/Context.hpp"
#include "client/Colours.hpp"

#include "client/OpenGL.hpp"

namespace oz
{
namespace client
{

  int ExplosionImago::modelId;

  Pool<ExplosionImago> ExplosionImago::pool;

  Imago* ExplosionImago::create( const Object* obj )
  {
    ExplosionImago* imago = new ExplosionImago();

    modelId = library.modelIndex( "explosion" );

    imago->obj = obj;
    imago->smm = context.requestSMM( modelId );
    imago->startMillis = timer.millis;

    return imago;
  }

  ExplosionImago::~ExplosionImago()
  {
    context.releaseSMM( modelId );
  }

  void ExplosionImago::draw( const Imago*, int mask )
  {
    if( !smm->isLoaded || !( mask & Mesh::ALPHA_BIT ) ) {
      return;
    }

    float millis = float( timer.millis - startMillis );
    float radius = millis * obj->dim.z * 0.004f;
    float alpha  = 1.0f - 0.002f * millis;

    glDisable( GL_CULL_FACE );

    shader.colour = Vec4( 1.0f, 1.0f, 1.0f, alpha*alpha );

    tf.model.scale( Vec3( radius, radius, radius ) );
    tf.apply();

    smm->draw( Mesh::SOLID_BIT );

    shader.colour = Colours::WHITE;

    glEnable( GL_CULL_FACE );
  }

}
}
