/*
 * OpenZone - Simple Cross-Platform FPS/RTS Game Engine
 * Copyright (C) 2002-2011  Davorin Učakar
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Davorin Učakar
 * <davorin.ucakar@gmail.com>
 */

/**
 * @file client/MD2Imago.cpp
 */

#include "stable.hpp"

#include "client/MD2Imago.hpp"

#include "matrix/Bot.hpp"

#include "client/Colours.hpp"
#include "client/Context.hpp"
#include "client/Camera.hpp"
#include "client/OpenGL.hpp"

namespace oz
{
namespace client
{

const float MD2Imago::TURN_SMOOTHING_COEF = 0.80f;

Pool<MD2Imago, 256> MD2Imago::pool;

Imago* MD2Imago::create( const Object* obj )
{
  hard_assert( obj->flags & Object::BOT_BIT );

  const Bot* bot   = static_cast<const Bot*>( obj );
  MD2Imago*  imago = new MD2Imago( obj );

  imago->flags = Imago::MD2MODEL_BIT;
  imago->md2   = context.requestMD2( obj->clazz->imagoModel );
  imago->h     = bot->h;

  imago->setAnim( bot->anim );
  imago->anim.nextFrame = imago->anim.endFrame;
  imago->anim.currFrame = imago->anim.endFrame;

  return imago;
}

MD2Imago::~MD2Imago()
{
  context.releaseMD2( clazz->imagoModel );
}

void MD2Imago::setAnim( Anim::Type type_ )
{
  int type = int( type_ );

  anim.type       = type_;
  anim.repeat     = MD2::ANIM_LIST[type].repeat;
  anim.startFrame = MD2::ANIM_LIST[type].firstFrame;
  anim.endFrame   = MD2::ANIM_LIST[type].lastFrame;
  anim.nextFrame  = anim.startFrame == anim.endFrame ? anim.endFrame : anim.startFrame + 1;

  anim.currTime   = 0.0f;

  if( type_ == Anim::ATTACK ) {
    const Bot*    bot    = static_cast<const Bot*>( obj );
    const Weapon* weapon = static_cast<const Weapon*>( orbis.objects[bot->weapon] );

    if( weapon != null ) {
      const WeaponClass* clazz = static_cast<const WeaponClass*>( weapon->clazz );

      anim.fps       = MD2::ANIM_LIST[type].fps * 0.5f / clazz->shotInterval;
      anim.frameTime = 1.0f / anim.fps;
    }
  }
  else {
    anim.fps       = MD2::ANIM_LIST[type].fps;
    anim.frameTime = 1.0f / anim.fps;
  }
}

void MD2Imago::draw( const Imago* parent, int mask )
{
  flags |= UPDATED_BIT;

  if( !md2->isLoaded ) {
    return;
  }

  const Bot*      bot   = static_cast<const Bot*>( obj );
  const BotClass* clazz = static_cast<const BotClass*>( bot->clazz );

  if( mask & Mesh::SOLID_BIT ) {
    if( bot->anim != anim.type ) {
      setAnim( bot->anim );
    }

    md2->advance( &anim, timer.frameTime );

    if( !( bot->state & Bot::DEAD_BIT ) ) {
      if( bot->index == camera.bot && !camera.isExternal ) {
        if( parent == null && bot->weapon != -1 && orbis.objects[bot->weapon] != null ) {
          h = bot->h;

          tf.model = Mat44::translation( obj->p - Point3::ORIGIN );
          tf.model.rotateZ( bot->h );

          tf.model.translate( Vec3( 0.0f, 0.0f, +bot->camZ ) );
          tf.model.rotateX( bot->v - Math::TAU / 4.0f );
          tf.model.translate( Vec3( 0.0f, 0.0f, -bot->camZ ) );

          context.drawImago( orbis.objects[bot->weapon], this, Mesh::SOLID_BIT );
        }
      }
      else {
        if( shader.mode == Shader::SCENE && parent == null ) {
          if( bot->h - h > Math::TAU / 2.0f ) {
            h = bot->h + TURN_SMOOTHING_COEF * ( h + Math::TAU - bot->h );
          }
          else if( h - bot->h > Math::TAU / 2.0f ) {
            h = bot->h + Math::TAU + TURN_SMOOTHING_COEF * ( h - bot->h - Math::TAU );
          }
          else {
            h = bot->h + TURN_SMOOTHING_COEF * ( h - bot->h );
          }
          h = Math::mod( h + Math::TAU, Math::TAU );

          tf.model = Mat44::translation( obj->p - Point3::ORIGIN );
          tf.model.rotateZ( h );
        }

        if( bot->state & Bot::CROUCHING_BIT ) {
          tf.model.translate( Vec3( 0.0f, 0.0f, clazz->dim.z - clazz->crouchDim.z ) );
        }

        md2->draw( &anim );

        if( parent == null && bot->weapon!= -1 && orbis.objects[bot->weapon] != null ) {
          context.drawImago( orbis.objects[bot->weapon], this, Mesh::SOLID_BIT );
        }
      }
    }
  }
  else if( bot->state & Bot::DEAD_BIT ) {
    shader.colour.w = min( bot->life * 8.0f / clazz->life, 1.0f );

    if( shader.mode == Shader::SCENE && parent == null ) {
      tf.model = Mat44::translation( obj->p - Point3::ORIGIN );
      tf.model.rotateZ( h );
    }
    tf.model.translate( Vec3( 0.0f, 0.0f, clazz->dim.z - clazz->corpseDim.z ) );

    md2->draw( &anim );

    shader.colour.w = 1.0f;
  }
}

}
}
