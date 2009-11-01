/*
 *  MD2Model.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#include "precompiled.h"

#include "MD2Model.h"

#include "matrix/BotClass.h"
#include "Context.h"

namespace oz
{
namespace client
{

  Model *MD2Model::create( const Object *obj )
  {
    assert( obj->flags & Object::BOT_BIT );

    const Bot *bot = static_cast<const Bot*>( obj );
    MD2Model *model = new MD2Model();
    MD2::AnimState &anim = model->anim;

    model->obj = obj;
    model->md2 = context.loadMD2Model( obj->type->modelName );

    anim.type       = bot->anim;
    anim.repeat     = MD2::animList[bot->anim].repeat;

    anim.startFrame = MD2::animList[bot->anim].firstFrame;
    anim.endFrame   = MD2::animList[bot->anim].lastFrame;
    anim.nextFrame  = anim.endFrame;
    anim.currFrame  = anim.endFrame;

    anim.fps        = MD2::animList[bot->anim].fps;
    anim.frameTime  = 1.0f / anim.fps;
    anim.currTime   = 0.0f;

    return model;
  }

  MD2Model::~MD2Model()
  {
    context.releaseMD2Model( obj->type->modelName );
  }

  void MD2Model::setAnim( int type )
  {
    if( anim.type != type ) {
      anim.type       = type;
      anim.repeat     = MD2::animList[type].repeat;

      anim.startFrame = MD2::animList[type].firstFrame;
      anim.endFrame   = MD2::animList[type].lastFrame;
      anim.nextFrame  = anim.startFrame + 1;

      anim.fps        = MD2::animList[type].fps;
      anim.frameTime  = 1.0f / anim.fps;
      anim.currTime   = 0.0f;
    }
  }

  void MD2Model::draw()
  {
    const Bot *bot = static_cast<const Bot*>( obj );
    const BotClass *clazz = static_cast<const BotClass*>( bot->type );

    if( bot->state & Bot::DEATH_BIT ) {
      float alpha = bot->life / clazz->life * 3.0f;
      glColor4f( 1.0f, 1.0f, 1.0f, alpha );
    }
    glRotatef( bot->h, 0.0f, 0.0f, 1.0f );

    setAnim( bot->anim );
    md2->draw( &anim );
  }

}
}
