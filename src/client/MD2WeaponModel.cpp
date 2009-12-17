/*
 *  MD2Weapon.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3.0. See COPYING for details.
 */

#include "precompiled.h"

#include "MD2WeaponModel.h"

#include "matrix/BotClass.h"
#include "Context.h"
#include "Colors.h"

namespace oz
{
namespace client
{

  Model *MD2WeaponModel::create( const Object *obj )
  {
    assert( obj->flags & Object::BOT_BIT );

    const Bot *bot = static_cast<const Bot*>( obj );
    MD2WeaponModel *model = new MD2WeaponModel();

    model->obj = obj;
    model->md2 = context.loadMD2( obj->type->modelName );

    model->setAnim( bot->anim );
    model->anim.nextFrame = model->anim.endFrame;
    model->anim.currFrame = model->anim.endFrame;

    return model;
  }

  MD2WeaponModel::~MD2WeaponModel()
  {
    context.releaseMD2( obj->type->modelName );
  }

  void MD2WeaponModel::setAnim( int type )
  {
    anim.type       = type;
    anim.repeat     = MD2::animList[type].repeat;

    anim.startFrame = MD2::animList[type].firstFrame;
    anim.endFrame   = MD2::animList[type].lastFrame;
    anim.nextFrame  = anim.startFrame + 1;

    anim.fps        = MD2::animList[type].fps;
    anim.frameTime  = 1.0f / anim.fps;
    anim.currTime   = 0.0f;
  }

  void MD2WeaponModel::draw()
  {
    const Bot *bot = static_cast<const Bot*>( obj );
    const BotClass *clazz = static_cast<const BotClass*>( bot->type );

    glRotatef( bot->h, 0.0f, 0.0f, 1.0f );

    if( bot->anim != anim.type ) {
      setAnim( bot->anim );
    }

    if( bot->state & Bot::DEATH_BIT ) {
      float color[] = { 1.0f, 1.0f, 1.0f, bot->life / clazz->life * 3.0f };

      glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color );
      md2->draw( &anim );
      glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, Colors::WHITE );
    }
    else {
      md2->draw( &anim );
    }
  }

}
}
