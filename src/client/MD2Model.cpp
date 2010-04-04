/*
 *  MD2Model.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING for details.
 */

#include "stable.h"

#include "client/MD2Model.h"

#include "matrix/BotClass.h"
#include "client/Colors.h"
#include "client/Context.h"
#include "client/Camera.h"
#include "client/Render.h"

namespace oz
{
namespace client
{

  Pool<MD2Model, 0, 256> MD2Model::pool;

  Model* MD2Model::create( const Object* obj )
  {
    assert( obj->flags & Object::BOT_BIT );

    const Bot* bot = static_cast<const Bot*>( obj );
    MD2Model* model = new MD2Model();

    model->obj   = obj;
    model->flags = Model::MD2MODEL_BIT;
    model->md2   = context.loadMD2( obj->type->modelName );

    model->setAnim( bot->anim );
    model->anim.nextFrame = model->anim.endFrame;
    model->anim.currFrame = model->anim.endFrame;

    return model;
  }

  MD2Model::~MD2Model()
  {
    context.releaseMD2( obj->type->modelName );
  }

  void MD2Model::setAnim( int type )
  {
    anim.type       = type;
    anim.repeat     = MD2::animList[type].repeat;

    anim.startFrame = MD2::animList[type].firstFrame;
    anim.endFrame   = MD2::animList[type].lastFrame;
    anim.nextFrame  = anim.startFrame == anim.endFrame ? anim.endFrame : anim.startFrame + 1;

    anim.fps        = MD2::animList[type].fps;
    anim.frameTime  = 1.0f / anim.fps;
    anim.currTime   = 0.0f;
  }

  void MD2Model::draw( const Model* )
  {
    const Bot* bot = static_cast<const Bot*>( obj );
    const BotClass* clazz = static_cast<const BotClass*>( bot->type );

    glRotatef( bot->h, 0.0f, 0.0f, 1.0f );

    if( bot->anim != anim.type ) {
      setAnim( bot->anim );
    }

    if( bot->state & Bot::DEATH_BIT ) {
      float color[] = { 1.0f, 1.0f, 1.0f, bot->life / clazz->life * 3.0f };

      glEnable( GL_BLEND );
      glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color );

      md2->advance( &anim, timer.frameTime );
      md2->draw( &anim );

      glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, Colors::WHITE );
      glDisable( GL_BLEND );
    }
    else if( bot->index != camera.bot || camera.isExternal ) {
      if( bot->state & Bot::CROUCHING_BIT ) {
        glTranslatef( 0.0f, 0.0f, clazz->dim.z - clazz->dimCrouch.z );
      }

      md2->advance( &anim, timer.frameTime );
      md2->draw( &anim );

      if( bot->weaponItem != -1 && world.objects[bot->weaponItem] != null ) {
        render.drawModel( world.objects[bot->weaponItem], this );
      }
    }
    else if( bot->weaponItem != -1 && world.objects[bot->weaponItem] != null ) {
      glTranslatef( 0.0f, 0.0f,  bot->camZ );
      glRotatef( bot->v, 1.0f, 0.0f, 0.0f );
      glTranslatef( 0.0f, 0.0f, -bot->camZ );

      md2->advance( &anim, timer.frameTime );
      render.drawModel( world.objects[bot->weaponItem], this );
    }
  }

}
}
