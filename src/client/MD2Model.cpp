/*
 *  MD2Model.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#include "precompiled.h"

#include "MD2Model.h"

#include "Context.h"

namespace oz
{
namespace client
{

  Model *MD2Model::create( const Object *obj )
  {
    assert( obj->flags & Object::BOT_BIT );

    MD2Model *model = new MD2Model();

    model->obj = obj;
    model->md2 = context.loadMD2Model( obj->type->modelName );

    model->anim.type     = -1;
    model->anim.currTime = 0.0f;
    model->anim.oldTime  = 0.0f;
    model->setAnim( 0 );

    return model;
  }

  MD2Model::~MD2Model()
  {
    context.releaseMD2Model( obj->type->modelName );
  }

  void MD2Model::setAnim( int type )
  {
    if( anim.type != type ) {
      anim.startFrame = MD2::animList[type].firstFrame;
      anim.endFrame   = MD2::animList[type].lastFrame;
      anim.currFrame  = anim.startFrame;
      anim.nextFrame  = anim.startFrame + 1;
      anim.fps        = MD2::animList[type].fps;
      anim.frameTime  = 1.0f / anim.fps;
      anim.type       = type;
    }
  }

  void MD2Model::draw()
  {
    const Bot *bot = (const Bot*) obj;

    glRotatef( bot->h, 0.0f, 0.0f, 1.0f );

    setAnim( bot->anim );
    md2->draw( &anim );
  }

}
}
