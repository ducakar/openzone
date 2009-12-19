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

#include "matrix/Bot.h"
#include "matrix/World.h"
#include "Context.h"
#include "Colors.h"

namespace oz
{
namespace client
{

  Model *MD2WeaponModel::create( const Object *obj )
  {
    assert( obj->flags & Object::DYNAMIC_BIT );
    assert( obj->flags & Object::WEAPON_BIT );

    MD2WeaponModel *model = new MD2WeaponModel();

    model->obj = obj;
    model->md2 = context.loadMD2( obj->type->modelName );

    model->setAnim( Bot::ANIM_STAND );
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
    md2->drawFrame( 0 );
  }

  void MD2WeaponModel::drawMounted()
  {
    const Dynamic *dyn = static_cast<const Dynamic*>( obj );
    const Bot *bot = static_cast<const Bot*>( world.objects[dyn->parent] );

    assert( bot->flags & Object::BOT_BIT );

    if( bot->anim != anim.type ) {
      setAnim( bot->anim );
    }
    md2->advance( &anim, timer.frameTime );
    md2->draw( &anim );
  }

}
}
