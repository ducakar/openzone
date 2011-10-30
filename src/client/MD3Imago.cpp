/*
 *  MD2Imago.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "client/MD3Imago.hpp"

#include "matrix/BotClass.hpp"

#include "client/Colours.hpp"
#include "client/Context.hpp"
#include "client/Camera.hpp"
#include "client/OpenGL.hpp"

namespace oz
{
namespace client
{

const float MD3Imago::TURN_SMOOTHING_COEF = 0.60f;

Pool<MD3Imago> MD3Imago::pool;

Imago* MD3Imago::create( const Object* obj )
{
  hard_assert( obj->flags & Object::BOT_BIT );

  const Bot* bot = static_cast<const Bot*>( obj );
  MD3Imago* imago = new MD3Imago();

  imago->obj   = obj;
  imago->flags = Imago::MD3MODEL_BIT;
  imago->clazz = obj->clazz;
  imago->md3   = context.requestMD3( obj->clazz->imagoModel );
  imago->h     = bot->h;

//     imago->setAnim( bot->anim );
//     imago->anim.nextFrame = imago->anim.endFrame;
//     imago->anim.currFrame = imago->anim.endFrame;

  return imago;
}

MD3Imago::~MD3Imago()
{
  context.releaseMD3( clazz->imagoModel );
}

//   void MD3Imago::setAnim( Anim::Type type_ )
//   {
//     int type = int( type_ );
//
//     anim.type       = type_;
//     anim.repeat     = MD3::ANIM_LIST[type].repeat;
//
//     anim.startFrame = MD3::ANIM_LIST[type].firstFrame;
//     anim.endFrame   = MD3::ANIM_LIST[type].lastFrame;
//     anim.nextFrame  = anim.startFrame == anim.endFrame ? anim.endFrame : anim.startFrame + 1;
//
//     anim.currTime   = 0.0f;
//
//     if( type_ == Anim::ATTACK ) {
//       const Bot*    bot    = static_cast<const Bot*>( obj );
//       const Weapon* weapon = static_cast<const Weapon*>( orbis.objects[bot->weapon] );
//
//       if( weapon != null ) {
//         const WeaponClass* clazz = static_cast<const WeaponClass*>( weapon->clazz );
//
//         anim.fps       = MD2::ANIM_LIST[type].fps * 0.5f / clazz->shotInterval;
//         anim.frameTime = 1.0f / anim.fps;
//       }
//     }
//     else {
//       anim.fps       = MD2::ANIM_LIST[type].fps;
//       anim.frameTime = 1.0f / anim.fps;
//     }
//   }

void MD3Imago::draw( const Imago* parent, int mask )
{
  const Bot* bot = static_cast<const Bot*>( obj );
  const BotClass* clazz = static_cast<const BotClass*>( bot->clazz );

  if( !md3->isLoaded ) {
    return;
  }

//     if( bot->anim != anim.type ) {
//       setAnim( bot->anim );
//     }

  if( parent == null ) {
    if( !camera.isExternal ) {
      h = bot->h;
    }
    else if( bot->h - h > Math::TAU / 2.0f ) {
      h = bot->h + TURN_SMOOTHING_COEF * ( h + Math::TAU - bot->h );
    }
    else if( h - bot->h > Math::TAU / 2.0f ) {
      h = bot->h + Math::TAU + TURN_SMOOTHING_COEF * ( h - bot->h - Math::TAU );
    }
    else {
      h = bot->h + TURN_SMOOTHING_COEF * ( h - bot->h );
    }

    h = Math::mod( h + Math::TAU, Math::TAU );
    tf.model.rotateZ( h );
  }

  if( bot->state & Bot::DEAD_BIT ) {
    shader.colour.w = min( bot->life * 8.0f / clazz->life, 1.0f );
    if( shader.colour.w != 1.0f ) {
      glEnable( GL_BLEND );
    }

    tf.model.translate( Vec3( 0.0f, 0.0f, clazz->dim.z - clazz->corpseDim.z ) );

//       md3->advance( &anim, timer.frameTime );
//       md3->draw( &anim );
    md3->drawFrame( 0 );

    if( shader.colour.w != 1.0f ) {
      glDisable( GL_BLEND );
    }
    shader.colour.w = 1.0f;
  }
  else if( bot->index != camera.bot || camera.isExternal ) {
    if( bot->state & Bot::CROUCHING_BIT ) {
      tf.model.translate( Vec3( 0.0f, 0.0f, clazz->dim.z - clazz->crouchDim.z ) );
    }

//       md3->advance( &anim, timer.frameTime );
//       md3->draw( &anim );
    md3->drawFrame( 0 );

    if( parent == null && bot->weapon!= -1 && orbis.objects[bot->weapon] != null ) {
      context.drawImago( orbis.objects[bot->weapon], this, mask );
    }
  }
  else if( parent == null && bot->weapon != -1 && orbis.objects[bot->weapon] != null ) {
    tf.model.translate( Vec3( 0.0f, 0.0f,  bot->camZ ) );
    tf.model.rotateX( bot->v - Math::TAU / 4.0f );
    tf.model.translate( Vec3( 0.0f, 0.0f, -bot->camZ ) );

    glDepthFunc( GL_ALWAYS );

//       md2->advance( &anim, timer.frameTime );
    context.drawImago( orbis.objects[bot->weapon], this, mask );

    glDepthFunc( GL_LESS );
  }
}

}
}
